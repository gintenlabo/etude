//
//  compare_equal_to のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/utility/compare_equal_to.hpp"

#include <boost/test/minimal.hpp>
#include <type_traits>
#include <utility>

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

struct convertible_to_bool
{
  bool value;
  
  explicit convertible_to_bool( bool b )
    : value( b ) {}
  
  // explicitly convertible to bool
  explicit operator bool() const { return value; }
  
};

struct X
{
  int value;
  
  explicit X( int value_ )
    : value( value_ ) {}
  
  friend convertible_to_bool operator==( X const& lhs, X const& rhs ) {
    return convertible_to_bool( lhs.value == rhs.value );
  }
  
};

struct Y
{
  int value;
  
  explicit Y( int value_ )
    : value( value_ ) {}
  
  // Y は X と、この順のみ比較可能
  friend convertible_to_bool operator==( Y const& lhs, X const& rhs ) {
    return convertible_to_bool( lhs.value == rhs.value );
  }
  
};

int test_main( int, char** )
{
  BOOST_CHECK(  etude::compare_equal_to( 0, 0 ) );
  BOOST_CHECK( !etude::compare_equal_to( 0, 1 ) );
  
  void* const p = 0;
  BOOST_CHECK(  etude::compare_equal_to(  p, (int*)0 ) );
  BOOST_CHECK( !etude::compare_equal_to( &p, (void*)0 ) );
  // etude::compare_equal_to( &p, (int*)0 );  // no match function
  
  X x1(1), x2(2);
  STATIC_ASSERT((
    !std::is_convertible<decltype( x1 == x2 ), bool>::value
  ));
  STATIC_ASSERT((
    std::is_same<decltype( etude::compare_equal_to( x1, x2 ) ), bool>::value
  ));
  BOOST_CHECK(  etude::compare_equal_to( x1, x1 ) );
  BOOST_CHECK( !etude::compare_equal_to( x1, x2 ) );
  
  Y y1(1), y2(2);
  STATIC_ASSERT((
    !std::is_convertible<decltype( y1 == x2 ), bool>::value
  ));
  STATIC_ASSERT((
    std::is_same<decltype( etude::compare_equal_to( x1, y1 ) ), bool>::value
  ));
  BOOST_CHECK(  etude::compare_equal_to( x1, y1 ) );
  BOOST_CHECK( !etude::compare_equal_to( x1, y2 ) );
  BOOST_CHECK(  etude::compare_equal_to( y1, x1 ) );
  BOOST_CHECK( !etude::compare_equal_to( y1, x2 ) );
  
  return 0;
}
