//
//  compare_less のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/utility/compare_less.hpp"

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
  
  friend convertible_to_bool operator<( X const& lhs, X const& rhs ) {
    return convertible_to_bool( lhs.value < rhs.value );
  }
  
};

int test_main( int, char** )
{
  BOOST_CHECK( !etude::compare_less( 0, 0 ) );
  BOOST_CHECK(  etude::compare_less( 0, 1 ) );
  BOOST_CHECK( !etude::compare_less( 1, 0 ) );
  
  void* const p = 0;
  BOOST_CHECK( !etude::compare_less( p, (int*)0 ) );
  BOOST_CHECK( !etude::compare_less( (int*)0, p ) );
  BOOST_CHECK(
    etude::compare_less( &p, (void*)0 ) || etude::compare_less( (void*)0, &p )
  );
  // etude::compare_less( &p, (int*)0 );  // no match function
  
  X x(1), y(2);
  STATIC_ASSERT((
    !std::is_convertible<decltype( x < y ), bool>::value
  ));
  STATIC_ASSERT((
    std::is_same<decltype( etude::compare_less( x, y ) ), bool>::value
  ));
  BOOST_CHECK( !etude::compare_less( x, x ) );
  BOOST_CHECK(  etude::compare_less( x, y ) );
  BOOST_CHECK( !etude::compare_less( y, x ) );
  
  return 0;
}
