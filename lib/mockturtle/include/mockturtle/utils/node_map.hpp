/* mockturtle: C++ logic network library
 * Copyright (C) 2018  EPFL
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

/*!
  \file node_map.hpp
  \brief Map indexed by network nodes
  \author Mathias Soeken
  \author Heinz Riener
*/

#pragma once

#include <cassert>
#include <memory>
#include <unordered_map>
#include <vector>

#include "../traits.hpp"

namespace mockturtle
{

/*! \brief Associative container network nodes
 *
 * This container helps to store and access values associated to nodes
 * in a network.
 *
 * Two implementations are provided one using std::vector and another
 * using std::unordered_map as internal storage.  The former
 * implementation can be pre-allocated and provides a fast way to
 * access the data.  The later implementation offers a way to
 * associate values to a subset of nodes and to check whether a value
 * is available.
 *
 * Example
 *
   \verbatim embed:rst
   .. code-block:: c++
      aig_network aig = ...
      node_map<std::string, aig_network> node_names( aig );
      aig.foreach_node( [&]( auto n ) {
        node_names[n] = "some string";
      } );
   \endverbatim
 */
    template<class T, class Ntk, class Impl = std::vector<T>>
    class node_map;

/*! \brief Vector node map
 *
 * This container is initialized with a network to derive the size
 * according to the number of nodes.  The container can be accessed
 * via nodes, or indirectly via signals, from which the corresponding
 * node is derived.
 *
 * The implementation uses a vector as underlying data structure which
 * is indexed by the node's index.
 *
 * **Required network functions:**
 * - `size`
 * - `get_node`
 * - `node_to_index`
 *
 */
    template<class T, class Ntk>
    class node_map<T, Ntk, std::vector<T>>
    {
    public:
        using node = typename Ntk::node;
        using signal = typename Ntk::signal;

        using reference = typename std::vector<T>::reference;
        using const_reference = typename std::vector<T>::const_reference;
    public:
        /*! \brief Default constructor. */
        explicit node_map( Ntk const& ntk )
                : ntk( ntk ),
                  data( std::make_shared<std::vector<T>>( ntk.size() ) )
        {
            static_assert( is_network_type_v<Ntk>, "Ntk is not a network type" );
            static_assert( has_size_v<Ntk>, "Ntk does not implement the size method" );
            static_assert( has_get_node_v<Ntk>, "Ntk does not implement the get_node method" );
            static_assert( has_node_to_index_v<Ntk>, "Ntk does not implement the node_to_index method" );
        }

        /*! \brief Constructor with default value.
         *
         * Initializes all values in the container to `init_value`.
         */
        node_map( Ntk const& ntk, T const& init_value )
                : ntk( ntk ),
                  data( std::make_shared<std::vector<T>>( ntk.size(), init_value ) )
        {
            static_assert( is_network_type_v<Ntk>, "Ntk is not a network type" );
            static_assert( has_size_v<Ntk>, "Ntk does not implement the size method" );
            static_assert( has_get_node_v<Ntk>, "Ntk does not implement the get_node method" );
            static_assert( has_node_to_index_v<Ntk>, "Ntk does not implement the node_to_index method" );
        }

        /*! \brief Mutable access to value by node. */
        reference operator[]( node const& n )
        {
            return (*data)[ntk.node_to_index( n )];
        }

        /*! \brief Constant access to value by node. */
        const_reference operator[]( node const& n ) const
        {
            assert( ntk.node_to_index( n ) < data->size() && "index out of bounds" );
            return (*data)[ntk.node_to_index( n )];
        }

        /*! \brief Mutable access to value by signal.
         *
         * This method derives the node from the signal.  If the node and signal type
         * are the same in the network implementation, this method is disabled.
         */
        template<typename _Ntk = Ntk, typename = std::enable_if_t<!std::is_same_v<typename _Ntk::signal, typename _Ntk::node>>>
        reference operator[]( signal const& f )
        {
            assert( ntk.node_to_index( ntk.get_node( f ) ) < data->size() && "index out of bounds" );
            return (*data)[ntk.node_to_index( ntk.get_node( f ) )];
        }

        /*! \brief Constant access to value by signal.
         *
         * This method derives the node from the signal.  If the node and signal type
         * are the same in the network implementation, this method is disabled.
         */
        template<typename _Ntk = Ntk, typename = std::enable_if_t<!std::is_same_v<typename _Ntk::signal, typename _Ntk::node>>>
        const_reference operator[]( signal const& f ) const
        {
            assert( ntk.node_to_index( ntk.get_node( f ) ) < data->size() && "index out of bounds" );
            return (*data)[ntk.node_to_index( ntk.get_node( f ) )];
        }

        /*! \brief Resets the size of the map.
         *
         * This function should be called, if the network changed in size.  Then, the
         * map is cleared, and resized to the current network's size.  All values are
         * initialized with `init_value`.
         *
         * \param init_value Initialization value after resize
         */
        void reset( T const& init_value = {} )
        {
            data->clear();
            data->resize( ntk.size(), init_value );
        }

        /*! \brief Resizes the map.
         *
         * This function should be called, if the node_map's size needs to
         * be changed without clearing its data.
         *
         * \param init_value Initialization value after resize
         */
        void resize( T const& init_value = {} )
        {
            if ( ntk.size() > data->size() )
            {
                data->resize( ntk.size(), init_value );
            }
        }

    private:
        Ntk const& ntk;
        std::shared_ptr<std::vector<T>> data;
    };

/*! \brief Unordered node map
 *
 * This implementation of the container is initialized with a network.
 * The map entries are constructed on the fly.  The container
 * can be accessed via ndoes, or indirectly via signals, from which
 * the corresponding node is derived.
 *
 * The implementation uses an std::unordered_map as underlying data
 * structure which is indexed by the node's index.
 *
 * **Required network functions:**
 * - `get_node`
 * - `node_to_index`
 *
 */
    template<class T, class Ntk>
    class node_map<T, Ntk, std::unordered_map<typename Ntk::node, T>>
    {
    public:
        using node = typename Ntk::node;
        using signal = typename Ntk::signal;

        using reference = T&;
        using const_reference = const T&;

    public:
        explicit node_map( Ntk const& ntk )
                : ntk( ntk ),
                  data( std::make_shared<std::unordered_map<typename Ntk::node, T>>() )
        {
        }

        /*! \brief Check if a key is already defined. */
        bool has( node const& n ) const
        {
            return data->find( n ) != data->end();
        }

        /*! \brief Mutable access to value by node. */
        reference operator[]( node const& n )
        {
            return (*data)[ntk.node_to_index( n )];
        }

        /*! \brief Constant access to value by node. */
        const_reference operator[]( node const& n ) const
        {
            assert( !has( n ) && "index out of bounds" );
            return (*data)[ntk.node_to_index( n )];
        }

        /*! \brief Mutable access to value by signal.
         *
         * This method derives the node from the signal.  If the node and signal type
         * are the same in the network implementation, this method is disabled.
         */
        template<typename _Ntk = Ntk, typename = std::enable_if_t<!std::is_same_v<typename _Ntk::signal, typename _Ntk::node>>>
        reference operator[]( signal const& f )
        {
            return (*data)[ntk.node_to_index( ntk.get_node( f ) )];
        }

        /*! \brief Constant access to value by signal.
         *
         * This method derives the node from the signal.  If the node and signal type
         * are the same in the network implementation, this method is disabled.
         */
        template<typename _Ntk = Ntk, typename = std::enable_if_t<!std::is_same_v<typename _Ntk::signal, typename _Ntk::node>>>
        const_reference operator[]( signal const& f ) const
        {
            assert( !has( ntk.get_node( f ) ) && "index out of bounds" );
            return (*data)[ntk.node_to_index( ntk.get_node( f ) )];
        }

        /*! \brief Resets the size of the map.
         *
         * This function should be called, if the network changed in size.  Then, the
         * map is cleared, and resized to the current network's size.
         */
        void reset()
        {
            data->clear();
        }

    protected:
        Ntk const& ntk;
        std::shared_ptr<std::unordered_map<node, T>> data;
    };

/*! \brief Template alias `unordered_node_map` */
    template<class T, class Ntk>
    using unordered_node_map = node_map<T, Ntk, std::unordered_map<typename Ntk::node, T>>;

} /* namespace mockturtle */