//
//  by_ref のテストです。
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/utility/by_ref.hpp"

#include <boost/utility/addressof.hpp>
#include <boost/assert.hpp>
#include <type_traits>
#include <utility>

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

template<class T>
inline void check( T x )
{
  // 型チェック
  /*
  // エラー
  STATIC_ASSERT((
    std::is_same<
      decltype( etude::by_ref( std::declval<T>() ) ), void
    >::value
  ));
  */
  STATIC_ASSERT((
    std::is_same<
      decltype( etude::by_ref( std::declval<T&>() ) ), T&
    >::value
  ));
  /*
  // エラー
  STATIC_ASSERT((
    std::is_same<
      decltype( etude::by_ref( std::declval<T const&>() ) ), void
    >::value
  ));
  */
  
  // アドレスチェック
  auto & t = etude::by_ref(x);
  BOOST_ASSERT( boost::addressof(t) == boost::addressof(x) );
}


class klass {};

int main()
{
  check( int() );
  check( klass() );
}
