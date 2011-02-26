//
//  optional のテストです。
//    
//    TODO: ちゃんと書く
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../etude/optional.hpp"

template class etude::optional<int>;
template class etude::optional<int&>;

#include <type_traits>
#include <boost/test/minimal.hpp>

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

#include <boost/utility/addressof.hpp>
template<class T>
inline bool is_same_object( T& x, T& y )
{
  return boost::addressof(x) == boost::addressof(y);
}

template<class LHS, class RHS>
inline bool checked_equal( LHS const& lhs, RHS const& rhs )
{
  bool result = ( lhs == rhs );
  BOOST_CHECK( ( lhs != rhs ) == !result );
  return result;
}

template<class LHS, class RHS>
inline bool checked_less( LHS const& lhs, RHS const& rhs )
{
  BOOST_CHECK( ( lhs >  rhs ) ==  ( rhs <  lhs ) );
  BOOST_CHECK( ( rhs >  lhs ) ==  ( lhs <  rhs ) );
  BOOST_CHECK( ( lhs >= rhs ) ==  ( rhs <= lhs ) );
  BOOST_CHECK( ( rhs >= lhs ) ==  ( lhs <= rhs ) );
  
  BOOST_CHECK( ( lhs >= rhs ) == !( lhs <  rhs ) );
  BOOST_CHECK( ( rhs >= lhs ) == !( rhs <  lhs ) );
  BOOST_CHECK( ( lhs <= rhs ) == !( lhs >  rhs ) );
  BOOST_CHECK( ( rhs <= lhs ) == !( rhs >  lhs ) );
  
  return lhs < rhs;
}

void check_compare()
{
  etude::optional<int> x0, y0, x1 = 1, y1 = 1, x2 = 2;
  
  BOOST_CHECK(  checked_equal( x0, x0 ) );
  BOOST_CHECK( !checked_less ( x0, x0 ) );
  
  BOOST_CHECK(  checked_equal( x0, y0 ) );
  BOOST_CHECK( !checked_less ( x0, y0 ) );
  
  BOOST_CHECK(  checked_equal( x0, boost::none ) );
  BOOST_CHECK( !checked_less ( x0, boost::none ) );
  
  BOOST_CHECK( !checked_equal( x0, 1 ) );
  BOOST_CHECK(  checked_less ( x0, 1 ) );
  BOOST_CHECK( !checked_less ( 1, x0 ) );
  
  BOOST_CHECK( !checked_equal( x0, x1 ) );
  BOOST_CHECK(  checked_less ( x0, x1 ) );
  BOOST_CHECK( !checked_less ( x1, x0 ) );
  
  BOOST_CHECK(  checked_equal( x1, x1 ) );
  BOOST_CHECK( !checked_less ( x1, x1 ) );
  
  BOOST_CHECK(  checked_equal( x1, y1 ) );
  BOOST_CHECK( !checked_less ( x1, y1 ) );
  
  BOOST_CHECK( !checked_equal( x1, x2 ) );
  BOOST_CHECK(  checked_less ( x1, x2 ) );
  BOOST_CHECK( !checked_less ( x2, x1 ) );
  
  BOOST_CHECK( !checked_equal( x1, boost::none ) );
  BOOST_CHECK( !checked_less ( x1, boost::none ) );
  BOOST_CHECK(  checked_less ( boost::none, x1 ) );
  
  BOOST_CHECK(  checked_equal( x1, 1 ) );
  BOOST_CHECK( !checked_less ( x1, 1 ) );
  BOOST_CHECK( !checked_less ( 1, x1 ) );
  
  BOOST_CHECK( !checked_equal( x1, 2 ) );
  BOOST_CHECK(  checked_less ( x1, 2 ) );
  BOOST_CHECK( !checked_less ( 2, x1 ) );
  
  BOOST_CHECK( !checked_equal( x1, -1 ) );
  BOOST_CHECK( !checked_less ( x1, -1 ) );
  BOOST_CHECK(  checked_less ( -1, x1 ) );
}

#include <boost/none.hpp>
int test_main( int, char** )
{
  etude::optional<int> x;
  BOOST_CHECK( x == boost::none );
  
  int i = 0;
  x = etude::make_optional(i);
  
  BOOST_CHECK( x && *x == 0 && !is_same_object( *x, i ) );
  
  x = 1;
  etude::optional<int> y;
  swap( x, y );
  BOOST_CHECK( !x && y && y == 1 );
  
  check_compare();
  
  return 0;
}
