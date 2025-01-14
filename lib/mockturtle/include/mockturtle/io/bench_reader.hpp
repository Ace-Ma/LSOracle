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
  \file bench_reader.hpp
  \brief Lorina reader for BENCH files

  \author Mathias Soeken
*/

#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <kitty/constructors.hpp>
#include <kitty/dynamic_truth_table.hpp>
#include <lorina/bench.hpp>

#include "../traits.hpp"

namespace mockturtle
{

/*! \brief Lorina reader callback for BENCH files.
 *
 * **Required network functions:**
 * - `create_pi`
 * - `create_po`
 * - `get_constant`
 * - `create_node`
 *
   \verbatim embed:rst
  
   Example
   
   .. code-block:: c++
   
      klut_network klut;
      lorina::read_bench( "file.bench", bench_reader( klut ) );
   \endverbatim
 */
template<typename Ntk>
class bench_reader : public lorina::bench_reader
{
public:
  explicit bench_reader( Ntk& ntk ) : _ntk( ntk )
  {
    static_assert( is_network_type_v<Ntk>, "Ntk is not a network type" );
    static_assert( has_create_pi_v<Ntk>, "Ntk does not implement the create_pi function" );
    static_assert( has_create_po_v<Ntk>, "Ntk does not implement the create_po function" );
    static_assert( has_get_constant_v<Ntk>, "Ntk does not implement the get_constant function" );
    static_assert( has_create_node_v<Ntk>, "Ntk does not implement the create_node function" );

    signals["gnd"] = _ntk.get_constant( false );
    signals["vdd"] = _ntk.get_constant( true );
  }

  ~bench_reader()
  {
    for ( auto const& o : outputs )
    {
      _ntk.create_po( signals[o], o );
    }
  }

  void on_input( const std::string& name ) const override
  {
    signals[name] = _ntk.create_pi( name );
  }

  void on_output( const std::string& name ) const override
  {
    outputs.emplace_back( name );
  }

  void on_assign( const std::string& input, const std::string& output ) const override
  {
    signals[output] = signals[input];
  }

  void on_gate( const std::vector<std::string>& inputs, const std::string& output, const std::string& type ) const override
  {
    if ( type.size() > 2 && std::string_view( type ).substr( 0, 2 ) == "0x" && inputs.size() <= 6u )
    {
      kitty::dynamic_truth_table tt( inputs.size() );
      kitty::create_from_hex_string( tt, type.substr( 2 ) );

      std::vector<signal<Ntk>> input_signals;
      for ( const auto& i : inputs )
      {
        input_signals.push_back( signals[i] );
      }

      signals[output] = _ntk.create_node( input_signals, tt );
    }
    else
    {
      assert( false );
    }
  }

private:
  Ntk& _ntk;

  mutable std::map<std::string, signal<Ntk>> signals;
  mutable std::vector<std::string> outputs;
};

} /* namespace mockturtle */
