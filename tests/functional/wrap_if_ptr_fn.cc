//
//  wrap_if_ptr_fn のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#include "../../etude/functional/wrap_if_ptr_fn.hpp"

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
  // 関数ポインタ
  {
    auto f_ = etude::wrap_if_ptr_fn( f );
    typedef decltype(etude::ptr_fn(f)) ptr_fn_to_f;
    
    STATIC_ASSERT((
      std::is_same<decltype(f_), ptr_fn_to_f>::value
    ));
    STATIC_ASSERT((
      std::is_same<typename ptr_fn_to_f::result_type, int>::value
    ));
    
    BOOST_CHECK( f_() == 42 );
    
    // && で束縛すると…
    auto && g_ = etude::wrap_if_ptr_fn( g );
    typedef decltype(etude::ptr_fn(g)) ptr_fn_to_g;
    
    STATIC_ASSERT((
      std::is_same<decltype(g_), ptr_fn_to_g&&>::value
    ));
    STATIC_ASSERT((
      std::is_same<typename ptr_fn_to_g::result_type, double>::value
    ));
    STATIC_ASSERT((
      std::is_same<typename ptr_fn_to_g::argument_type, int>::value
    ));
    
    BOOST_CHECK( g_(11) == g(11) );
  }
  
  // メンバポインタ
  {
    X x(1);
    
    // rvalue
    auto pm = etude::wrap_if_ptr_fn( &X::member );
    
    STATIC_ASSERT((
      std::is_same<decltype(pm), decltype(std::mem_fn(&X::member))>::value
    ));
    BOOST_CHECK( pm(x) == x.member );
    
    // lvalue
    auto foo = &X::foo;
    auto pf = etude::wrap_if_ptr_fn( foo );
    typedef decltype(pf) pf_t;
    
    STATIC_ASSERT((
      std::is_same<decltype(pf), decltype(std::mem_fn(foo))>::value
    ));
    STATIC_ASSERT((
      std::is_same<typename pf_t::result_type, int>::value
    ));
    STATIC_ASSERT((
      std::is_same<typename pf_t::argument_type, X const*>::value
    ));
    
    BOOST_CHECK( pf(x) == x.member );
    
    // const lvalue
    auto const bar = &X::bar;
    auto pb = etude::wrap_if_ptr_fn( bar );
    typedef decltype(pb) pb_t;
    
    STATIC_ASSERT((
      std::is_same<decltype(pb), decltype(std::mem_fn(bar))>::value
    ));
    STATIC_ASSERT((
      std::is_same<typename pb_t::result_type, void>::value
    ));
    STATIC_ASSERT((
      std::is_same<typename pb_t::first_argument_type, X*>::value
    ));
    STATIC_ASSERT((
      std::is_same<typename pb_t::second_argument_type, int>::value
    ));
    
    pb( &x, 1 );
    BOOST_CHECK( x.member == 2 );
  }
  
  // ファンクタ
  {
    std::plus<int> const plus = {};
    auto && plus_ = etude::wrap_if_ptr_fn( plus );
    STATIC_ASSERT((
      std::is_same<decltype(plus_), std::plus<int> const&>::value
    ));
    BOOST_CHECK( plus_( 1, 1 ) == 2 );
  }
  
  return 0;
}
