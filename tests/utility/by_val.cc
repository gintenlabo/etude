//
//  by_val のテストです。
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/utility/by_val.hpp"

#include <boost/utility/addressof.hpp>
#include <boost/assert.hpp>
#include <type_traits>
#include <utility>

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

template<class T>
inline void check( T x )
{
  // 型チェック
  STATIC_ASSERT((
    std::is_same<
      decltype( etude::by_val( std::declval<T>() ) ), T&&
    >::value
  ));
  STATIC_ASSERT((
    std::is_same<
      decltype( etude::by_val( std::declval<T&>() ) ), T const&
    >::value
  ));
  STATIC_ASSERT((
    std::is_same<
      decltype( etude::by_val( std::declval<T const&>() ) ), T const&
    >::value
  ));
  
  // アドレスチェック
  auto && v = etude::by_val(x);
  BOOST_ASSERT( boost::addressof(v) == boost::addressof(x) );
}


class klass {};

int main()
{
  check( int() );
  check( klass() );
}
