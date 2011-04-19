//
//  pipe_operator のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/functional/pipe_operator.hpp"
#include "../../etude/functional/group.hpp"

#include <type_traits>
#include <boost/test/minimal.hpp>
#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

#include <tuple>
#include <utility>
#include <functional>

template<class... Args>
class sum_;

template<class T>
struct sum_<T>
{
  typedef typename std::decay<T>::type result_type;
  
  result_type operator()( T x ) const {
    return std::forward<T>(x);
  }
  
};

template<class T, class U, class... Args>
struct sum_<T, U, Args...>
{
  typedef decltype( std::declval<T>() + std::declval<U>() ) TU_;
  typedef sum_<TU_&&, Args&&...> tail_;
  typedef typename tail_::result_type result_type;
  
  result_type operator()( T t, U u, Args... args ) const {
    return tail_()(
      std::forward<T>(t) + std::forward<U>(u), std::forward<Args>(args)...
    );
  }
  
};

struct sum_t
{
  template<class... Args,
    class Result = typename sum_<Args&&...>::result_type
  >
  Result operator()( Args&&... args ) const {
    return sum_<Args&&...>()( std::forward<Args>(args)... );
  }
  
};

sum_t const sum = {};

#include "../../etude/functional/ptr_fn.hpp"

int f( int x ) {
  return x * 2;
}

bool is_nullptr( void const volatile* p )
{
  return p == 0;
}

struct X
{
  int i;
  void f( int d ){ i += d; }
};

int test_main( int, char** )
{
  using etude::pipe_operator::operator|;
  
  {
    // 関数オブジェクト
    auto r = etude::group( 1, 2, 3 ) | sum;
    STATIC_ASSERT(( std::is_same<decltype(r), int>::value ));
    BOOST_CHECK( r == 6 );
  }
  {
    // 関数ポインタ
    int i = 2;
    // 組み込み型を関数ポインタに渡したい場合は、関数をクラスにラップする
    auto r = i | etude::ptr_fn(f);
    STATIC_ASSERT(( std::is_same<decltype(r), int>::value ));
    BOOST_CHECK( r == 4 );
  }
  {
    // 0 を渡しても正しく推論してくれる！
    auto r = 0 | etude::ptr_fn(is_nullptr);
    STATIC_ASSERT(( std::is_same<decltype(r), bool>::value ));
    BOOST_CHECK( r == true );
  }
  {
    // メンバポインタ
    X x = {1};
    auto pi = &X::i;
    auto pf = &X::f;
    
    auto i = x | pi;
    STATIC_ASSERT(( std::is_same<decltype(i), int>::value ));
    BOOST_CHECK( i == x.i );
    
    etude::group( x, 2 ) | pf;
    BOOST_CHECK( x.i == 3 );
  }
  
  return 0;
}
