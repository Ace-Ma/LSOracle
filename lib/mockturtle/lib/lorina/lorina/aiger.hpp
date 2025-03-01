/* lorina: C++ parsing library
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
  \file aig.hpp
  \brief Implements aiger parser

  \author Heinz Riener
*/

#pragma once

#include <lorina/common.hpp>
#include <lorina/diagnostics.hpp>
#include <lorina/detail/utils.hpp>
#include <fstream>
#include <regex>
#include <iostream>

namespace lorina
{

/*! \brief A reader visitor for the binary AIGER format.
 *
 * Callbacks for the AIGER format.
 */
class aiger_reader
{
public:
  /*! Latch input values */
  enum latch_init_value
  {
    ZERO = 0 /*!< Initialized with 0 */
  , ONE /*!< Initialized with 1 */
  , NONDETERMINISTIC /*!< Not initialized (non-deterministic value) */
  };

public:
  /*! \brief Callback method for parsed header.
   *
   * \param m Maximum variable index
   * \param i Number of inputs
   * \param l Number of latches
   * \param o Number of outputs
   * \param a Number of AND gates
   */
  virtual void on_header( std::size_t m, std::size_t i, std::size_t l, std::size_t o, std::size_t a ) const
  {
    (void)m;
    (void)i;
    (void)l;
    (void)o;
    (void)a;
  }

  /*! \brief Callback method for parsed header.
   *
   * \param m Maximum variable index
   * \param i Number of inputs
   * \param l Number of latches
   * \param o Number of outputs
   * \param a Number of AND gates
   * \param b Number of bad states properties
   * \param c Number of invariant constraints
   * \param j Number of justice properties
   * \param f Number of fairness constraints
   */
  virtual void on_header( std::size_t m, std::size_t i, std::size_t l, std::size_t o, std::size_t a,
                          std::size_t b, std::size_t c, std::size_t j, std::size_t f ) const
  {
    on_header( m, i, l, o, a );
    (void)b;
    (void)c;
    (void)j;
    (void)f;
  }

  /*! \brief Callback method for parsed input.
   *
   * \param index Index of the input
   * \param lit Assigned literal
   */
  virtual void on_input( unsigned index, unsigned lit ) const
  {
    (void)index;
    (void)lit;
  }

  /*! \brief Callback method for parsed output.
   *
   * \param index Index of the output
   * \param lit Assigned literal
   */
  virtual void on_output( unsigned index, unsigned lit ) const
  {
    (void)index;
    (void)lit;
  }

  /*! \brief Callback method for parsed latch.
   *
   * \param index Index of the latch
   * \param next Assigned (next) literal
   * \param reset Initial value of the latch
   */
  virtual void on_latch( unsigned index, unsigned next, latch_init_value reset ) const
  {
    (void)index;
    (void)next;
    (void)reset;
  }

  /*! \brief Callback method for parsed AND gate.
   *
   * \param index Index of the AND gate
   * \param left_lit Assigned left literal
   * \param right_lit Assigned right literal
   */
  virtual void on_and( unsigned index, unsigned left_lit, unsigned right_lit ) const
  {
    (void)index;
    (void)left_lit;
    (void)right_lit;
  }

    /*! \brief Callback method for parsed bad state property.
     *
     * \param index Index of the bad state property
     * \param lit Assigned literal
     */
    virtual void on_bad_state( unsigned index, unsigned lit ) const
    {
        (void)index;
        (void)lit;
    }

    /*! \brief Callback method for parsed constraint.
     *
     * \param index Index of the constraint
     * \param lit Assigned literal
     */
    virtual void on_constraint( unsigned index, unsigned lit ) const
    {
        (void)index;
        (void)lit;
    }

    /*! \brief Callback method for parsed fairness constraints.
     *
     * \param index Index of the fairness constraint
     * \param lit Assigned literal
     */
    virtual void on_fairness( unsigned index, unsigned lit ) const
    {
        (void)index;
        (void)lit;
    }

    /*! \brief Callback method for parsed header of justice property.
     *
     * \param index Index of the justice property
     * \param lit Number of assigned literals
     */
    virtual void on_justice_header( unsigned index, std::size_t size ) const
    {
        (void)index;
        (void)size;
    }

    /*! \brief Callback method for parsed justice property.
     *
     * \param index Index of the justice property
     * \param lit Assigned literal
     */
    virtual void on_justice( unsigned index, const std::vector<unsigned>& lits ) const
    {
        (void)index;
        (void)lits;
    }

  /*! \brief Callback method for parsed input name.
   *
   * \param index Index of the input
   * \param name Input name
   */
  virtual void on_input_name( unsigned index, const std::string& name ) const
  {
    (void)index;
    (void)name;
  }

  /*! \brief Callback method for parsed latch name.
   *
   * \param index Index of the latch
   * \param name Latch name
   */
  virtual void on_latch_name( unsigned index, const std::string& name ) const
  {
    (void)index;
    (void)name;
  }

  /*! \brief Callback method for parsed output name.
   *
   * \param index Index of the output
   * \param name Output name
   */
  virtual void on_output_name( unsigned index, const std::string& name ) const
  {
    (void)index;
    (void)name;
  }

  /*! \brief Callback method for a parsed name of a bad state property.
   *
   * \param index Index of the bad state property
   * \param name Name of the bad state property
   */
  virtual void on_bad_state_name( unsigned index, const std::string& name ) const
  {
    (void)index;
    (void)name;
  }

  /*! \brief Callback method for a parsed name of an invariant constraint.
   *
   * \param index Index of the constraint
   * \param name Constraint name
   */
  virtual void on_constraint_name( unsigned index, const std::string& name ) const
  {
    (void)index;
    (void)name;
  }
    /*! \brief Callback method for a parsed name of a justice property.
      *
      * \param index Index of the justice property
      * \param name Name of the fairness constraint
      */
    virtual void on_justice_name( unsigned index, const std::string& name ) const
    {
        (void)index;
        (void)name;
    }

  /*! \brief Callback method for a parsed name of a fairness constraint.
   *
   * \param index Index of the fairness constraint
   * \param name Name of the fairness constraint
   */
  virtual void on_fairness_name( unsigned index, const std::string& name ) const
  {
    (void)index;
    (void)name;
  }

  /*! \brief Callback method for parsed comment.
   *
   * \param comment Comment
   */
  virtual void on_comment( const std::string& comment ) const
  {
    (void)comment;
  }
}; /* aiger_reader */

/*! \brief An AIGER reader for prettyprinting ASCII AIGER.
 *
 * Callbacks for prettyprinting of ASCII AIGER.
 *
 */
class ascii_aiger_pretty_printer : public aiger_reader
{
public:
  /*! \brief Constructor of the ASCII AIGER pretty printer.
   *
   * \param os Output stream
   */
  ascii_aiger_pretty_printer( std::ostream& os = std::cout )
      : _os( os )
  {
  }

  void on_header( std::size_t m, std::size_t i, std::size_t l, std::size_t o, std::size_t a,
                  std::size_t b, std::size_t c, std::size_t j, std::size_t f ) const override
  {
    _os << fmt::format( "aag {0} {1} {2} {3} {4} {5} {6} {7} {8} ",
                        m, i, l, o, a, b, c, j, f )
        << std::endl;
  }

  void on_input( unsigned index, unsigned lit ) const override
  {
    (void)index;
    _os << lit << std::endl;
  }

  void on_output( unsigned index, unsigned lit ) const override
  {
    (void)index;
    _os << lit << std::endl;
  }

  void on_latch( unsigned index, unsigned next, latch_init_value init_value ) const override
  {
    _os << ( 2u * index ) << ' ' << next;
    switch( init_value )
    {
    case 0:
      _os << '0';
      break;
    case 1:
      _os << '1';
      break;
    default:
      break;
    }
    _os << std::endl;
  }

  void on_and( unsigned index, unsigned left_lit, unsigned right_lit ) const override
  {
    _os << ( 2u * index ) << ' ' << left_lit << ' ' << right_lit << std::endl;
  }

  void on_input_name( unsigned index, const std::string& name ) const override
  {
    _os << "i" << index << ' ' << name << std::endl;
  }

  void on_latch_name( unsigned index, const std::string& name ) const override
  {
    _os << "l" << index << ' ' << name << std::endl;
  }

  void on_output_name( unsigned index, const std::string& name ) const override
  {
    _os << "o" << index << ' ' << name << std::endl;
  }

  void on_bad_state_name( unsigned index, const std::string& name ) const override
  {
    _os << "b" << index << ' ' << name << std::endl;
  }

  void on_constraint_name( unsigned index, const std::string& name ) const override
  {
    _os << "c" << index << ' ' << name << std::endl;
  }

  void on_fairness_name( unsigned index, const std::string& name ) const override
  {
    _os << "f" << index << ' ' << name << std::endl;
  }

  void on_comment( const std::string& comment ) const override
  {
    _os << "c" << std::endl
        << comment << std::endl;
  }

  std::ostream& _os; /*!< Output stream */
}; /* ascii_aiger_pretty_printer */

namespace aig_regex
{
static std::regex header( R"(^aig (\d+) (\d+) (\d+) (\d+) (\d+)( \d+)?( \d+)?( \d+)?( \d+)?$)" );
static std::regex ascii_header( R"(^aag (\d+) (\d+) (\d+) (\d+) (\d+)( \d+)?( \d+)?( \d+)?( \d+)?$)" );
static std::regex input( R"(^i(\d+) (.*)$)" );
static std::regex latch( R"(^l(\d+) (.*)$)" );
static std::regex output( R"(^o(\d+) (.*)$)" );
static std::regex bad_state( R"(^b(\d+) (.*)$)" );
static std::regex constraint( R"(^c(\d+) (.*)$)" );
static std::regex fairness( R"(^f(\d+) (.*)$)" );
} // namespace aig_regex

/*! \brief Reader function for ASCII AIGER format.
 *
 * Reads ASCII AIGER format from a stream and invokes a callback
 * method for each parsed primitive and each detected parse error.
 *
 * \param in Input stream
 * \param reader An AIGER reader with callback methods invoked for parsed primitives
 * \param diag An optional diagnostic engine with callback methods for parse errors
 * \return Success if parsing have been successful, or parse error if parsing have failed
 */
inline return_code read_ascii_aiger( std::istream& in, const aiger_reader& reader, diagnostic_engine* diag = nullptr )
{
  return_code result = return_code::success;

  std::smatch m;
  std::string header_line;
  std::getline( in, header_line );

  std::size_t _m, _i, _l, _o, _a, _b, _c, _j, _f;

  /* header */
  if ( std::regex_search( header_line, m, aig_regex::ascii_header ) )
  {
    std::vector<std::size_t> header;
    for ( const auto& i : m )
    {
      if ( i == "" )
        continue;
      header.push_back( std::atol( std::string( i ).c_str() ) );
    }

    assert( header.size() >= 6u );
    assert( header.size() <= 10u );

    _m = header[1u];
    _i = header[2u];
    _l = header[3u];
    _o = header[4u];
    _a = header[5u];
    _b = header.size() > 6 ? header[6u] : 0ul;
    _c = header.size() > 7 ? header[7u] : 0ul;
    _j = header.size() > 8 ? header[8u] : 0ul;
    _f = header.size() > 9 ? header[9u] : 0ul;
    reader.on_header( _m, _i, _l, _o, _a, _b, _c, _j, _f );
  }
  else
  {
    if ( diag )
    {
      diag->report( diagnostic_level::fatal,
                    fmt::format( "could not parse AIGER header `{0}`", header_line ) );
    }
    return return_code::parse_error;
  }

  std::string line;

  /* inputs */
  for ( auto i = 0ul; i < _i; ++i )
  {
    std::getline( in, line );
    const auto index = std::atol( line.c_str() )/2u;
    reader.on_input( i, index );
  }

  /* latches */
  for ( auto i = 0ul; i < _l; ++i )
  {
    std::getline( in, line );
    const auto tokens = detail::split( line,  " " );
    assert( tokens.size() <= 3u );

    const auto index = std::atol( std::string(tokens[0u]).c_str() ) / 2u;
    const auto next_lit = std::atol( std::string(tokens[1u]).c_str() );

    aiger_reader::latch_init_value init_value = aiger_reader::latch_init_value::NONDETERMINISTIC;
    if ( tokens.size() == 3u )
    {
      if ( tokens[2u] == "0" )
      {
        init_value = aiger_reader::latch_init_value::ZERO;
      }
      else if ( tokens[1u] == "1" )
      {
        init_value = aiger_reader::latch_init_value::ONE;
      }
    }

    reader.on_latch( index, next_lit, init_value );
  }

  /* outputs */
  for ( auto i = 0ul; i < _o; ++i )
  {
    std::getline( in, line );
    const auto lit = std::atol( line.c_str() );
    reader.on_output( i, lit );
  }

  /* ands */
  for ( auto i = 0ul; i < _a; ++i )
  {
    std::getline( in, line );
    const auto tokens = detail::split( line, " " );
    assert( tokens.size() == 3u );
    const auto index = std::atol( std::string(tokens[0u]).c_str() )/2u;
    const auto left_lit = std::atol( std::string(tokens[1u]).c_str() );
    const auto right_lit = std::atol( std::string(tokens[2u]).c_str() );
    reader.on_and( index, left_lit, right_lit );
  }

  /* parse names and comments */
  while ( std::getline( in, line ) )
  {
    if ( std::regex_search( line, m, aig_regex::input ) )
    {
      reader.on_input_name( std::atol( std::string( m[1u] ).c_str() ), m[2u] );
    }
    else if ( std::regex_search( line, m, aig_regex::latch ) )
    {
      reader.on_latch_name( std::atol( std::string( m[1u] ).c_str() ), m[2u] );
    }
    else if ( std::regex_search( line, m, aig_regex::output ) )
    {
      reader.on_output_name( std::atol( std::string( m[1u] ).c_str() ), m[2u] );
    }
    else if ( std::regex_search( line, m, aig_regex::bad_state ) )
    {
      reader.on_bad_state_name( std::atol( std::string( m[1u] ).c_str() ), m[2u] );
    }
    else if ( std::regex_search( line, m, aig_regex::constraint ) )
    {
      reader.on_constraint_name( std::atol( std::string( m[1u] ).c_str() ), m[2u] );
    }
    else if ( std::regex_search( line, m, aig_regex::fairness ) )
    {
      reader.on_fairness_name( std::atol( std::string( m[1u] ).c_str() ), m[2u] );
    }
    else if ( line == "c" )
    {
      std::string comment = "";
      while ( std::getline( in, line ) )
      {
        comment += line;
      }
      reader.on_comment( comment );
      break;
    }
  }

  return result;
}

/*! \brief Reader function for ASCII AIGER format.
 *
 * Reads ASCII AIGER format from a file and invokes a callback
 * method for each parsed primitive and each detected parse error.
 *
 * \param filename Name of the file
 * \param reader An AIGER reader with callback methods invoked for parsed primitives
 * \param diag An optional diagnostic engine with callback methods for parse errors
 * \return Success if parsing have been successful, or parse error if parsing have failed
 */
inline return_code read_ascii_aiger( const std::string& filename, const aiger_reader& reader, diagnostic_engine* diag = nullptr )
{
  std::ifstream in( detail::word_exp_filename( filename ), std::ifstream::in );
  return read_ascii_aiger( in, reader, diag );
}

/*! \brief Reader function for binary AIGER format.
 *
 * Reads binary AIGER format from a stream and invokes a callback
 * method for each parsed primitive and each detected parse error.
 *
 * \param in Input stream
 * \param reader An AIGER reader with callback methods invoked for parsed primitives
 * \param diag An optional diagnostic engine with callback methods for parse errors
 * \return Success if parsing have been successful, or parse error if parsing have failed
 */
inline return_code read_aiger( std::istream& in, const aiger_reader& reader, diagnostic_engine* diag = nullptr )
{
  return_code result = return_code::success;

  std::smatch m;
  std::string header_line;
  std::getline( in, header_line );

  std::size_t _m, _i, _l, _o, _a, _b, _c, _j, _f;

  /* parse header */
  if ( std::regex_search( header_line, m, aig_regex::header ) )
  {
    std::vector<std::size_t> header;
    for ( const auto& i : m )
    {
      if ( i == "" )
        continue;
      header.push_back( std::atol( std::string( i ).c_str() ) );
    }

    assert( header.size() >= 6u );
    assert( header.size() <= 10u );

    _m = header[1u];
    _i = header[2u];
    _l = header[3u];
    _o = header[4u];
    _a = header[5u];
    _b = header.size() > 6 ? header[6u] : 0ul;
    _c = header.size() > 7 ? header[7u] : 0ul;
    _j = header.size() > 8 ? header[8u] : 0ul;
    _f = header.size() > 9 ? header[9u] : 0ul;
    reader.on_header( _m, _i, _l, _o, _a, _b, _c, _j, _f );
  }
  else
  {
    if ( diag )
    {
      diag->report( diagnostic_level::fatal,
                    fmt::format( "could not parse AIGER header `{0}`", header_line ) );
    }
    return return_code::parse_error;
  }

  std::string line;

  /* inputs */
  for ( auto i = 0ul; i < _i; ++i )
  {
    reader.on_input( i, 2u*(i+1) );
  }

  /* latches */
  for ( auto i = 0ul; i < _l; ++i )
  {
    std::getline( in, line );
    const auto tokens = detail::split( line, " " );
    const auto next = std::atoi( tokens[0u].c_str() );
    aiger_reader::latch_init_value init_value = aiger_reader::latch_init_value::NONDETERMINISTIC;
    if ( tokens.size() == 2u )
    {
      if ( tokens[1u] == "0" )
      {
        init_value = aiger_reader::latch_init_value::ZERO;
      }
      else if ( tokens[1u] == "1" )
      {
        init_value = aiger_reader::latch_init_value::ONE;
      }
    }

    reader.on_latch( _i + i + 1u, next, init_value );
  }

  for ( auto i = 0ul; i < _o; ++i )
  {
    std::getline( in, line );
    reader.on_output( i, std::atol( line.c_str() ) );
  }

    /* bad state properties */
    for ( auto i = 0ul; i < _b; ++i )
    {
        std::getline( in, line );
        reader.on_bad_state( i, std::atol( line.c_str() ) );
    }

    /* constraints */
    for ( auto i = 0ul; i < _c; ++i )
    {
        std::getline( in, line );
        reader.on_constraint( i, std::atol( line.c_str() ) );
    }

    /* justice properties */
    std::vector<std::size_t> justice_sizes;
    for ( auto i = 0ul; i < _j; ++i )
    {
        std::getline( in, line );
        const auto justice_size = std::atol( line.c_str() );
        justice_sizes.emplace_back( justice_size );
        reader.on_justice_header( i, justice_size );
    }

    for ( auto i = 0ul; i < _j; ++i )
    {
        std::vector<unsigned> lits;
        for ( auto j = 0ul; j < justice_sizes[i]; ++j )
        {
            std::getline( in, line );
            const auto lit = std::atol( line.c_str() );
            lits.emplace_back( lit );
        }
        reader.on_justice( i, lits );
    }

    /* fairness */
    for ( auto i = 0ul; i < _f; ++i )
    {
        std::getline( in, line );
        reader.on_fairness( i, std::atol( line.c_str() ) );
    }

  const auto decode = [&]() {
    auto i = 0;
    auto res = 0;
    while ( true )
    {
      auto c = in.get();

      res |= ( ( c & 0x7f ) << ( 7 * i ) );
      if ( ( c & 0x80 ) == 0 )
        break;

      ++i;
    }
    return res;
  };

  /* and gates */
  for ( auto i = _i + _l + 1; i < _i + _l + _a + 1; ++i )
  {
    const auto d1 = decode();
    const auto d2 = decode();
    const auto g = i << 1;
    reader.on_and( i, ( g - d1 ), ( g - d1 - d2 ) );
  }

  /* parse names and comments */
  while ( std::getline( in, line ) )
  {
    if ( std::regex_search( line, m, aig_regex::input ) )
    {
      reader.on_input_name( std::atol( std::string( m[1u] ).c_str() ), m[2u] );
    }
    else if ( std::regex_search( line, m, aig_regex::latch ) )
    {
      reader.on_latch_name( std::atol( std::string( m[1u] ).c_str() ), m[2u] );
    }
    else if ( std::regex_search( line, m, aig_regex::output ) )
    {
      reader.on_output_name( std::atol( std::string( m[1u] ).c_str() ), m[2u] );
    }
    else if ( std::regex_search( line, m, aig_regex::bad_state ) )
    {
      reader.on_bad_state_name( std::atol( std::string( m[1u] ).c_str() ), m[2u] );
    }
    else if ( std::regex_search( line, m, aig_regex::constraint ) )
    {
      reader.on_constraint_name( std::atol( std::string( m[1u] ).c_str() ), m[2u] );
    }
    else if ( std::regex_search( line, m, aig_regex::fairness ) )
    {
      reader.on_fairness_name( std::atol( std::string( m[1u] ).c_str() ), m[2u] );
    }
    else if ( line == "c" )
    {
      std::string comment = "";
      while ( std::getline( in, line ) )
      {
        comment += line;
      }
      reader.on_comment( comment );
      break;
    }
  }

  return result;
}

/*! \brief Reader function for binary AIGER format.
 *
 * Reads binary AIGER format from a file and invokes a callback
 * method for each parsed primitive and each detected parse error.
 *
 * \param filename Name of the file
 * \param reader An AIGER reader with callback methods invoked for parsed primitives
 * \param diag An optional diagnostic engine with callback methods for parse errors
 * \return Success if parsing have been successful, or parse error if parsing have failed
 */
inline return_code read_aiger( const std::string& filename, const aiger_reader& reader, diagnostic_engine* diag = nullptr )
{
  std::ifstream in( detail::word_exp_filename( filename ), std::ifstream::in );
  return read_aiger( in, reader, diag );
}

} // namespace lorina
