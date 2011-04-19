//
//  pointee_before のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/utility/pointee_before.hpp"

#include <boost/test/minimal.hpp>
#include <type_traits>
#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

template<class T, class U>
void check_( T const& x, U const& y, bool less, bool greater )
{
  BOOST_CHECK( etude::pointee_before( x, y ) == less );
  BOOST_CHECK( etude::pointee_before( y, x ) == greater );
}

template<class T> inline T const* make_const( T* p ){ return p; }

template<class T, class U>
void check( T* x, U* y, bool less, bool greater )
{
  check_( x,            y,  less, greater );
  check_( x, make_const(y), less, greater );
}
template<class T, class U, class Comp>
void check( T* x, U* y, Comp comp, bool less, bool greater )
{
  BOOST_CHECK( etude::pointee_before( x, y, comp ) == less );
  BOOST_CHECK( etude::pointee_before( y, x, comp ) == greater );
  BOOST_CHECK( etude::pointee_before( x, make_const(y), comp ) == less );
  BOOST_CHECK( etude::pointee_before( y, make_const(x), comp ) == greater );
}

#include <boost/optional.hpp> // 手本
#include <functional> // for std::greater

template<class T>
void check( boost::optional<T> const& x, boost::optional<T> const& y )
{
  check_( x, y, ( x < y ), ( y < x ) );
}


int test_main( int, char** )
{
  int i = 0, j = 0, k = 1;
  int* p = 0;
  
  // 一通りチェック
  check(  p,  p, false, false );
  check(  p, &i,  true, false );
  check( &i,  p, false,  true );
  check( &i, &j, false, false );
  check( &i, &k,  true, false );
  check( &k, &i, false,  true );
  
  // カスタム比較関数の場合（ Strict Weak Order なら何でもいい）
  std::greater<int> const greater = {};
  // 無効値との比較は比較関数に依らず一定
  check(  p,  p, greater, false, false );
  check(  p, &i, greater,  true, false );
  check( &i,  p, greater, false,  true );
  // 共に有効値の場合のみ比較関数が影響する（今回は greater なので反転）
  check( &i, &j, greater, false, false );
  check( &i, &k, greater, false,  true );
  check( &k, &i, greater,  true, false );
  
  // boost::optional による答え合わせ
  boost::optional<int> x, y = 1, z = 2;
  check( x, x );
  check( x, y );
  check( y, y );
  check( y, z );
  
  return 0;
}
