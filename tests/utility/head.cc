//
//  head のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/utility/head.hpp"

#include <type_traits>
#include <boost/utility/addressof.hpp>
#include <boost/assert.hpp>
#include "../../etude/utility/forward_as_tuple.hpp"
#include "../../etude/unpack.hpp"

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

template<class T, class... Args>
inline void check( T && x, Args&&... args )
{
  STATIC_ASSERT((
    std::is_same< T&&,
      decltype(
        etude::head( std::declval<T>(), std::declval<Args>()... )
      )
    >::value
  ));
  
  typedef std::tuple<T, Args...> tuple_type;
  
  STATIC_ASSERT((
    std::is_same< T&&,
      decltype(
        etude::head( etude::unpack( std::declval<tuple_type>() ) )
      )
    >::value
  ));
  
  // アドレスチェック
  auto && x1 = etude::head( std::forward<T>(x), std::forward<Args>(args)... );
  BOOST_ASSERT( boost::addressof(x1) == boost::addressof(x) );
  
  auto t = etude::forward_as_tuple( std::forward<T>(x), std::forward<Args>(args)... );
  auto && x2 = etude::head( etude::unpack( std::move(t) ) );
  BOOST_ASSERT( boost::addressof(x2) == boost::addressof(x) );
}

int main()
{
  int lvalue = 0;
  int const const_lvalue = 23;
  
  check( 1, lvalue, const_lvalue );
  check( lvalue, const_lvalue, 2 );
  check( const_lvalue, 3, lvalue );
  check( std::move(const_lvalue) );
}
