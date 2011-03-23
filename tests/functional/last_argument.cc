//
//  last_argument のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/functional/last_argument.hpp"

#include <type_traits>
#include <boost/utility/addressof.hpp>
#include <boost/test/minimal.hpp>
#include "../../etude/utility/forward_as_tuple.hpp"
#include "../../etude/unpack.hpp"
#include "../../etude/types/last_type.hpp"

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

template<class T, class U>
bool is_explicitly_same_object( T && x, U && y )
{
  return std::is_same<T, U>::value && ( boost::addressof(x) == boost::addressof(y) );
}


template<class... Args>
inline void check( Args&&... args )
{
  typedef typename etude::last_type<Args...>::type&& expected_type;
  
  STATIC_ASSERT((
    std::is_same< expected_type,
      decltype(
        etude::last_argument( std::declval<Args>()... )
      )
    >::value
  ));
  
  typedef std::tuple<Args...> tuple_type;
  
  STATIC_ASSERT((
    std::is_same< expected_type,
      decltype(
        etude::last_argument( etude::unpack( std::declval<tuple_type>() ) )
      )
    >::value
  ));
  
  // アドレスチェック
  auto && x1 = etude::last_argument( std::forward<Args>(args)... );
  
  auto t = etude::forward_as_tuple( std::forward<Args>(args)... );
  auto && x2 = etude::last_argument( etude::unpack( std::move(t) ) );
  
  BOOST_CHECK( is_explicitly_same_object( x1, x2 ) );
}

int test_main( int, char** )
{
  int lvalue = 0;
  int const const_lvalue = 23;
  
  check( 1, lvalue, const_lvalue );
  check( lvalue, const_lvalue, 2 );
  check( const_lvalue, 3, lvalue );
  check( std::move(const_lvalue) );
  
  // 空 tuple は無理
  // etude::last_argument( etude::unpack( std::make_tuple() ) );
  
  // group
  auto t = std::make_tuple( 0, std::ref(lvalue) );
  auto && x = etude::last_argument( const_lvalue, etude::unpack(t) );
  BOOST_CHECK( is_explicitly_same_object( std::forward<decltype(x)>(x), lvalue ) );
  
  return 0;
}
