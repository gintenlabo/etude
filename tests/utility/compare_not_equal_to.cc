//
//  compare_not_equal_to のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/utility/compare_not_equal_to.hpp"

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

// == 演算が定義されたクラス
struct X
{
  int value;
  
  explicit X( int value_ )
    : value( value_ ) {}
  
  friend convertible_to_bool operator==( X const& lhs, X const& rhs ) {
    return convertible_to_bool( lhs.value == rhs.value );
  }
  // operator != は定義されていない
  
};
// == と != が矛盾するクラス（ != が優先される）
struct Y
{
  int value;
  
  explicit Y( int value_ )
    : value( value_ ) {}
  
  friend convertible_to_bool operator==( Y const& lhs, Y const& rhs ) {
    return convertible_to_bool( lhs.value == rhs.value );
  }
  // != は常に true を返す
  friend convertible_to_bool operator!=( Y const&, Y const& ) {
    return convertible_to_bool( true );
  }
  
};
// 左右不対象な == ないし != を持ったクラス
struct Z
{
  int value;
  
  explicit Z( int value_ )
    : value( value_ ) {}
  
  // X に対しては operator== しか定義されない
  friend convertible_to_bool operator==( Z const& lhs, X const& rhs ) {
    return convertible_to_bool( lhs.value == rhs.value );
  }
  
  // Y に対しては矛盾した operator== と operator!= の組
  friend convertible_to_bool operator==( Z const& lhs, Y const& rhs ) {
    return convertible_to_bool( lhs.value == rhs.value );
  }
  // != は常に true を返す
  friend convertible_to_bool operator!=( Z const&, Y const& ) {
    return convertible_to_bool( true );
  }
  
};

int test_main( int, char** )
{
  BOOST_CHECK( !etude::compare_not_equal_to( 0, 0 ) );
  BOOST_CHECK(  etude::compare_not_equal_to( 0, 1 ) );
  
  void* const p = 0;
  BOOST_CHECK( !etude::compare_not_equal_to(  p, (int*)0 ) );
  BOOST_CHECK(  etude::compare_not_equal_to( &p, (void*)0 ) );
  // etude::compare_not_equal_to( &p, (int*)0 );  // no match function
  
  X x1(1), x2(2);
  STATIC_ASSERT((
    std::is_same<decltype( etude::compare_not_equal_to( x1, x2 ) ), bool>::value
  ));
  BOOST_CHECK( !etude::compare_not_equal_to( x1, x1 ) );
  BOOST_CHECK(  etude::compare_not_equal_to( x1, x2 ) );
  
  Y y1(1), y2(2);
  BOOST_CHECK( etude::compare_not_equal_to( y1, y1 ) );
  BOOST_CHECK( etude::compare_not_equal_to( y1, y2 ) );
  
  Z z(1);
  BOOST_CHECK( !etude::compare_not_equal_to( z, x1 ) );
  BOOST_CHECK(  etude::compare_not_equal_to( z, x2 ) );
  BOOST_CHECK( !etude::compare_not_equal_to( x1, z ) );
  BOOST_CHECK(  etude::compare_not_equal_to( x2, z ) );
  BOOST_CHECK(  etude::compare_not_equal_to( z, y1 ) );
  BOOST_CHECK(  etude::compare_not_equal_to( z, y2 ) );
  BOOST_CHECK(  etude::compare_not_equal_to( y1, z ) );
  BOOST_CHECK(  etude::compare_not_equal_to( y2, z ) );
  
  return 0;
}
