//
//  ptr_fn のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#include "../../etude/functional/ptr_fn.hpp"

#include <type_traits>
#include <boost/test/minimal.hpp>
#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

template<class F>
inline void check_ptr_fn( F f )
{
  // F は trivially-copyable な standard-layout class
  STATIC_ASSERT(( std::is_class<F>::value ));
  STATIC_ASSERT(( std::is_standard_layout<F>::value ));
  // STATIC_ASSERT(( std::is_trivially_copyable<F>::value ));
  
  // typename F::pointer は関数ポインタ
  typedef typename F::pointer pointer;
  STATIC_ASSERT(( std::is_pointer<pointer>::value ));
  STATIC_ASSERT((
    std::is_function<typename std::remove_pointer<pointer>::type>::value
  ));
  
  // sizeof(F) は sizeof(pointer)
  STATIC_ASSERT(( sizeof(F) == sizeof(pointer) ));
  
  // F cv f; と宣言された f に対し、
  // reinterpret_cast<F::pointer cv&>( f ); は well-formed で、その値は f.get() に等しい
  BOOST_CHECK(( f.get() == reinterpret_cast<pointer&>(f) ));
}

#include <cstdio>
#include <string>

double half( int x ) {
  return x * 0.5;
}

int test_main( int, char** )
{
  // 通常の場合のテスト
  {
    auto f = etude::ptr_fn( half );
    check_ptr_fn( f );
    
    // 型チェック
    typedef decltype(f) ptr_fn_t;
    STATIC_ASSERT(( std::is_same<ptr_fn_t::result_type, double>::value ));
    STATIC_ASSERT(( std::is_same<ptr_fn_t::argument_type, int>::value ));
    
    // 呼び出しチェック
    
    // rvalue
    BOOST_CHECK(( f(2) == 1.0 ));
    // lvalues
    int i = 3; BOOST_CHECK(( f(i) == 1.5 ));
    int const j = 23; BOOST_CHECK(( f(j) == 11.5 ));
    
    // conversion
    struct {
      operator int() const { return 42; }
    } x;
    BOOST_CHECK(( f(x) == 21 ));
    
    // aggregate
    BOOST_CHECK(( f({}) == 0 ));
  }
  
  // ... がある場合に対するテスト
  {
    auto f = etude::ptr_fn(std::sprintf);
    check_ptr_fn( f );
    char buf[16];
    
    // 余計な引数がない場合
    f( buf, "hoge" );
    BOOST_CHECK( std::string(buf) == "hoge" );
    
    // 余計な引数がある場合
    f( buf, "%d", 42 );
    BOOST_CHECK( std::string(buf) == "42" );
  }
  
  // メンバポインタに対するテスト
  {
    struct X {
      int i;
      int f() const { return i * 2; }
    };
    auto pi = &X::i;
    auto pf = &X::f;
    
    auto pi_ = etude::ptr_fn(pi);
    auto pf_ = etude::ptr_fn(pf);
    
    STATIC_ASSERT(( std::is_same<decltype(pi_), decltype(std::mem_fn(pi))>::value ));
    STATIC_ASSERT(( std::is_same<decltype(pf_), decltype(std::mem_fn(pf))>::value ));
    
    X x = {1};
    BOOST_CHECK( pi_(x) == x.i );
    BOOST_CHECK( pf_(x) == x.f() );
    
  }
  
  return 0;
}
