//
//  compare_greater_equal のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/utility/compare_greater_equal.hpp"

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
// <= と < が矛盾するクラス（ <= が優先される）
struct Y
{
  int value;
  
  explicit Y( int value_ )
    : value( value_ ) {}
  
  friend convertible_to_bool operator<( Y const& lhs, Y const& rhs ) {
    return convertible_to_bool( lhs.value < rhs.value );
  }
  // <= は常に true を返す（つまり <= も true になる）
  friend convertible_to_bool operator<=( Y const&, Y const& ) {
    return convertible_to_bool( true );
  }
  
};

// boost::logic::tribool を返す operator< がある場合
#include <boost/logic/tribool.hpp>
struct Z
{
  int value;
  
  explicit Z( int value_ )
    : value( value_ ) {}
  
  friend boost::logic::tribool operator<( Z const& lhs, Z const& rhs ) {
    if( lhs.value == 0 || rhs.value == 0 ) {
      return boost::logic::indeterminate; // 0 は比較できない
    }
    else {
      return lhs.value < rhs.value;
    }
  }
  
  
};

int test_main( int, char** )
{
  BOOST_CHECK(  etude::compare_greater_equal( 0, 0 ) );
  BOOST_CHECK( !etude::compare_greater_equal( 0, 1 ) );
  BOOST_CHECK(  etude::compare_greater_equal( 1, 0 ) );
  
  void* const p = 0;
  BOOST_CHECK(  etude::compare_greater_equal( p, (int*)0 ) );
  BOOST_CHECK(  etude::compare_greater_equal( (int*)0, p ) );
  BOOST_CHECK(
    etude::compare_greater_equal( &p, (void*)0 ) ||
    etude::compare_greater_equal( (void*)0, &p )
  );
  // etude::compare_greater_equal( &p, (int*)0 );  // no match function
  
  X x1(1), x2(2);
  STATIC_ASSERT((
    std::is_same<decltype( etude::compare_greater_equal( x1, x2 ) ), bool>::value
  ));
  BOOST_CHECK(  etude::compare_greater_equal( x1, x1 ) );
  BOOST_CHECK( !etude::compare_greater_equal( x1, x2 ) );
  BOOST_CHECK(  etude::compare_greater_equal( x2, x1 ) );
  
  Y y1(1), y2(2);
  BOOST_CHECK( etude::compare_greater_equal( y1, y1 ) );
  BOOST_CHECK( etude::compare_greater_equal( y1, y2 ) );
  BOOST_CHECK( etude::compare_greater_equal( y2, y1 ) );
  
  Z z0(0), z1(1), z2(2);
  BOOST_CHECK( !etude::compare_greater_equal( z0, z0 ) );
  BOOST_CHECK( !etude::compare_less( z0, z0 ) );  // 参考までに。
  BOOST_CHECK( !etude::compare_greater_equal( z0, z1 ) );
  BOOST_CHECK( !etude::compare_greater_equal( z0, z2 ) );
  BOOST_CHECK( !etude::compare_greater_equal( z1, z0 ) );
  BOOST_CHECK( !etude::compare_greater_equal( z2, z0 ) );
  BOOST_CHECK(  etude::compare_greater_equal( z1, z1 ) );
  BOOST_CHECK( !etude::compare_greater_equal( z1, z2 ) );
  BOOST_CHECK(  etude::compare_greater_equal( z2, z1 ) );
  
  return 0;
}
