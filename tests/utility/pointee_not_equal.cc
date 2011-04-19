//
//  pointee_not_equal のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/utility/pointee_not_equal.hpp"

#include <boost/test/minimal.hpp>
#include <type_traits>
#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

template<class T, class U>
void check_( T const& x, U const& y, bool expected )
{
  BOOST_CHECK( etude::pointee_not_equal( x, y ) == expected );
  BOOST_CHECK( etude::pointee_not_equal( y, x ) == expected );
}

template<class T> inline T const* make_const( T* p ){ return p; }

template<class T, class U>
void check( T* x, U* y, bool expected )
{
  check_( x,            y,  expected );
  check_( x, make_const(y), expected );
}
template<class T, class U, class Comp>
void check( T* x, U* y, Comp comp, bool expected )
{
  BOOST_CHECK( etude::pointee_not_equal( x, y, comp ) == expected );
  BOOST_CHECK( etude::pointee_not_equal( y, x, comp ) == expected );
  BOOST_CHECK( etude::pointee_not_equal( x, make_const(y), comp ) == expected );
  BOOST_CHECK( etude::pointee_not_equal( y, make_const(x), comp ) == expected );
}

#include <boost/optional.hpp> // 手本
#include <functional> // for std::not_equal_to

template<class T>
void check( boost::optional<T> const& x, boost::optional<T> const& y )
{
  check_( x, y, ( x != y ) );
}


int test_main( int, char** )
{
  int i = 0, j = 0, k = 1;
  int* p = 0;
  
  // 一通りチェック
  check(  p,  p, false );
  check(  p, &i, true );
  check( &i, &j, false );
  check( &i, &k, true );
  
  // カスタム比較関数の場合（例としてはあまりよくないが、 equal_to でチェック）
  std::equal_to<int> const equal_to = {};
  // 無効値との比較は比較関数に依らず一定
  check(  p,  p, equal_to, false );
  check(  p, &i, equal_to, true );
  // 共に有効値の場合のみ比較関数が影響する（今回は equal_to なので反転）
  check( &i, &j, equal_to, true );
  check( &i, &k, equal_to, false );
  
  // boost::optional による答え合わせ
  boost::optional<int> x, y = 1, z = 2;
  check( x, x );
  check( x, y );
  check( y, y );
  check( y, z );
  
  return 0;
}
