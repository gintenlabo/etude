//
//  copy のテストです。
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/utility/copy.hpp"

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
      decltype( etude::copy( std::declval<T>() ) ), T
    >::value
  ));
  STATIC_ASSERT((
    std::is_same<
      decltype( etude::copy( std::declval<T&>() ) ), T
    >::value
  ));
  STATIC_ASSERT((
    std::is_same<
      decltype( etude::copy( std::declval<T const&>() ) ), T
    >::value
  ));
  
  // アドレスチェック
  auto && t = etude::copy(x);
  BOOST_ASSERT( boost::addressof(t) != boost::addressof(x) );
}


class klass {};

int main()
{
  check( int() );
  check( klass() );
  
  // 文字列リテラルを始めとした配列の場合はポインタに変わる
  STATIC_ASSERT((
    std::is_same<
      decltype( etude::copy( "hoge" ) ), char const*
    >::value
  ));
}
