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
  \file node_resynthesis.hpp
  \brief Node resynthesis
  \author Mathias Soeken
*/

#pragma once

#include <iostream>

#include "../traits.hpp"
#include "../utils/node_map.hpp"
#include "../utils/stopwatch.hpp"
#include "../views/topo_view.hpp"

#include <fmt/format.h>

namespace mockturtle
{

/*! \brief Parameters for node_resynthesis.
 *
 * The data structure `node_resynthesis_params` holds configurable parameters
 * with default arguments for `node_resynthesis`.
 */
struct node_resynthesis_params
{
  /*! \brief Be verbose. */
  bool verbose{false};
};

/*! \brief Statistics for node_resynthesis.
 *
 * The data structure `node_resynthesis_stats` provides data collected by
 * running `node_resynthesis`.
 */
struct node_resynthesis_stats
{
  /*! \brief Total runtime. */
  stopwatch<>::duration time_total{0};

  void report() const
  {
    std::cout << fmt::format( "[i] total time = {:>5.2f} secs\n", to_seconds( time_total ) );
  }
};

namespace detail
{

template<class NtkDest, class NtkSource, class ResynthesisFn>
class node_resynthesis_impl
{
public:
  node_resynthesis_impl( NtkSource const& ntk, ResynthesisFn&& resynthesis_fn, node_resynthesis_params const& ps, node_resynthesis_stats& st )
    : ntk( ntk ),
      resynthesis_fn( resynthesis_fn ),
      ps( ps ),
      st( st )
  {
  }

  NtkDest run()
  {
    stopwatch t( st.time_total );

    NtkDest ntk_dest;
    node_map<signal<NtkDest>, NtkSource> node2new( ntk );
    // std::cout << "node2new size = " << node2new.size() << "\n";
    /* map constants */
    node2new[ntk.get_node( ntk.get_constant( false ) )] = ntk_dest.get_constant( false );
    if ( ntk.get_node( ntk.get_constant( true ) ) != ntk.get_node( ntk.get_constant( false ) ) )
    {
      node2new[ntk.get_node( ntk.get_constant( true ) )] = ntk_dest.get_constant( true );
    }

    /* map primary inputs */
    ntk.foreach_pi( [&]( auto n ) {
      //if(ntk.num_latches() > 0 && ntk.node_to_index( n ) > ntk.num_pis() - ntk.num_latches()) {
      //  std::cout << "Creating RO in resynthesis " << std::endl;
      //  node2new[n] = ntk_dest.create_ro();
      //}
      //else if (ntk.node_to_index( n ) <= ntk.num_pis() - ntk.num_latches()){
      node2new[n] = ntk_dest.create_pi();
      //}
    } );

    /* map nodes */
    topo_view ntk_topo{ntk};
    ntk_topo.foreach_node( [&]( auto n ) {
      if ( ntk.is_constant( n ) || ntk.is_pi( n ) || ntk.is_ci( n ) || ntk.is_ro( n ))
        return;

      std::vector<signal<NtkDest>> children;
      ntk.foreach_fanin( n, [&]( auto const& f ) {
        children.push_back( ntk.is_complemented( f ) ? ntk_dest.create_not( node2new[f] ) : node2new[f] );
      } );

      resynthesis_fn( ntk_dest, ntk.node_function( n ), children.begin(), children.end(), [&]( auto const& f ) {
        node2new[n] = f;
        return false;
      } );
    } );

    /* map primary outputs */
    ntk.foreach_po( [&]( auto const& f ) {
      ntk_dest.create_po( ntk.is_complemented( f ) ? ntk_dest.create_not( node2new[f] ) : node2new[f] );
    } );
//    ntk.foreach_po( [&]( auto const& f, auto i ) {
//      if (i < ntk.num_pos() - ntk.num_latches())
//        ntk_dest.create_po( ntk.is_complemented( f ) ? ntk_dest.create_not( node2new[f] ) : node2new[f] );
//      else if(ntk.num_latches()>0)
//        ntk_dest.create_ri( ntk.is_complemented( f ) ? ntk_dest.create_not( node2new[f] ) : node2new[f] );
//    });
    return ntk_dest;
  }

private:
  NtkSource const& ntk;
  ResynthesisFn&& resynthesis_fn;
  node_resynthesis_params const& ps;
  node_resynthesis_stats& st;
};

} /* namespace detail */

/*! \brief Node resynthesis algorithm.
 *
 * This algorithm takes as input a network (of type `NtkSource`) and creates a
 * new network (of type `NtkDest`), by translating each node of the input
 * network into a subnetwork for the output network.  To find a new subnetwork,
 * the algorithm uses a resynthesis function that takes as input the input
 * node's truth table.  This algorithm can for example be used to translate
 * k-LUT networks into AIGs or MIGs.
 *
 * The resynthesis function must be of type `NtkDest::signal(NtkDest&,
 * kitty::dynamic_truth_table const&, LeavesIterator, LeavesIterator)` where
 * `LeavesIterator` can be dereferenced to a `NtkDest::signal`.  The last two
 * parameters compose an iterator pair where the distance matches the number of
 * variables of the truth table that is passed as second parameter.
 *
 * **Required network functions for parameter ntk (type NtkSource):**
 * - `get_node`
 * - `get_constant`
 * - `foreach_pi`
 * - `foreach_node`
 * - `is_constant`
 * - `is_pi`
 * - `is_complemented`
 * - `foreach_fanin`
 * - `node_function`
 * - `foreach_po`
 *
 * **Required network functions for return value (type NtkDest):**
 * - `get_constant`
 * - `create_pi`
 * - `create_not`
 * - `create_po`
 *
 * \param ntk Input network of type `NtkSource`
 * \param resynthesis_fn Resynthesis function
 * \return An equivalent network of type `NtkDest`
 */
template<class NtkDest, class NtkSource, class ResynthesisFn>
NtkDest node_resynthesis( NtkSource const& ntk, ResynthesisFn&& resynthesis_fn, node_resynthesis_params const& ps = {}, node_resynthesis_stats* pst = nullptr )
{
  static_assert( is_network_type_v<NtkSource>, "NtkSource is not a network type" );
  static_assert( is_network_type_v<NtkDest>, "NtkDest is not a network type" );

  static_assert( has_get_node_v<NtkSource>, "NtkSource does not implement the get_node method" );
  static_assert( has_get_constant_v<NtkSource>, "NtkSource does not implement the get_constant method" );
  static_assert( has_foreach_pi_v<NtkSource>, "NtkSource does not implement the foreach_pi method" );
  static_assert( has_foreach_node_v<NtkSource>, "NtkSource does not implement the foreach_node method" );
  static_assert( has_is_constant_v<NtkSource>, "NtkSource does not implement the is_constant method" );
  static_assert( has_is_pi_v<NtkSource>, "NtkSource does not implement the is_pi method" );
  static_assert( has_is_complemented_v<NtkSource>, "NtkSource does not implement the is_complemented method" );
  static_assert( has_foreach_fanin_v<NtkSource>, "NtkSource does not implement the foreach_fanin method" );
  static_assert( has_node_function_v<NtkSource>, "NtkSource does not implement the node_function method" );
  static_assert( has_foreach_po_v<NtkSource>, "NtkSource does not implement the foreach_po method" );

  static_assert( has_get_constant_v<NtkDest>, "NtkDest does not implement the get_constant method" );
  static_assert( has_create_pi_v<NtkDest>, "NtkDest does not implement the create_pi method" );
  static_assert( has_create_not_v<NtkDest>, "NtkDest does not implement the create_not method" );
  static_assert( has_create_po_v<NtkDest>, "NtkDest does not implement the create_po method" );

  node_resynthesis_stats st;
  detail::node_resynthesis_impl<NtkDest, NtkSource, ResynthesisFn> p( ntk, resynthesis_fn, ps, st );
  const auto ret = p.run();
  if ( ps.verbose )
  {
    st.report();
  }

  if ( pst )
  {
    *pst = st;
  }
  return ret;
}

} // namespace mockturtle