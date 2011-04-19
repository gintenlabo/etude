//
//  wrap_if_mem_fn のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#include "../../etude/functional/wrap_if_mem_fn.hpp"

#include <type_traits>
#include <boost/test/minimal.hpp>
#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

struct X
{
  X() : member() {}
  explicit X( int x ) : member(x) {}
  
  int member;
  
  int foo() const { return member; }
  void bar( int i ) { member += i; }
  
};

// 普通の関数
int f() { return 42; }
double g( int i ) { return i / 2.0; }

int test_main( int, char** )
{
  // メンバポインタ
  {
    X x(1);
    
    // rvalue
    auto pm = etude::wrap_if_mem_fn( &X::member );
    STATIC_ASSERT((
      std::is_same<decltype(pm), decltype(std::mem_fn(&X::member))>::value
    ));
    BOOST_CHECK( pm(x) == x.member );
    
    // lvalue
    auto foo = &X::foo;
    auto pf = etude::wrap_if_mem_fn( foo );
    STATIC_ASSERT((
      std::is_same<decltype(pf), decltype(std::mem_fn(foo))>::value
    ));
    BOOST_CHECK( pf(x) == x.member );
    
    // const lvalue
    auto const bar = &X::bar;
    auto pb = etude::wrap_if_mem_fn( bar );
    STATIC_ASSERT((
      std::is_same<decltype(pb), decltype(std::mem_fn(bar))>::value
    ));
    pb( &x, 1 );
    BOOST_CHECK( x.member == 2 );
  }
  
  // メンバポインタ以外
  {
    auto f_ = etude::wrap_if_mem_fn( f );
    STATIC_ASSERT((
      std::is_same<decltype(f_), int (*)()>::value
    ));
    BOOST_CHECK( f_() == 42 );
    
    // && で束縛すると…
    auto && g_ = etude::wrap_if_mem_fn( g );
    STATIC_ASSERT((
      std::is_same<decltype(g_), double (&)(int)>::value
    ));
    BOOST_CHECK( g_(11) == g(11) );
    
    // ファンクタ
    std::plus<int> const plus = {};
    auto && plus_ = etude::wrap_if_mem_fn( plus );
    STATIC_ASSERT((
      std::is_same<decltype(plus_), std::plus<int> const&>::value
    ));
    BOOST_CHECK( plus_( 1, 1 ) == 2 );
  }
  
  return 0;
}
