//
//  get_nth_argument のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/functional/get_nth_argument.hpp"

#include <type_traits>
#include <tuple>
#include <boost/utility/addressof.hpp>
#include <boost/test/minimal.hpp>
#include "../../etude/utility/forward_as_tuple.hpp"
#include "../../etude/unpack.hpp"
#include "../../etude/types/nth_type.hpp"

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

template<class T, class U>
bool is_explicitly_same_object( T && x, U && y )
{
  return std::is_same<T, U>::value && ( boost::addressof(x) == boost::addressof(y) );
}

template<std::size_t N, class... Args>
inline void check( Args&&... args )
{
  typedef typename etude::nth_type<N, Args...>::type&& expected_type;
  
  STATIC_ASSERT((
    std::is_same< expected_type,
      decltype(
        etude::get_nth_argument<N>( std::declval<Args>()... )
      )
    >::value
  ));
  
  typedef std::tuple<Args...> tuple_type;
  
  STATIC_ASSERT((
    std::is_same< expected_type,
      decltype(
        etude::get_nth_argument<N>( etude::unpack( std::declval<tuple_type>() ) )
      )
    >::value
  ));
  
  // アドレスチェック
  auto && x1 = etude::get_nth_argument<N>( std::forward<Args>(args)... );
  
  auto t = etude::forward_as_tuple( std::forward<Args>(args)... );
  auto && x2 = etude::get_nth_argument<N>( etude::unpack( std::move(t) ) );
  
  BOOST_CHECK( is_explicitly_same_object( x1, x2 ) );
}

int test_main( int, char** )
{
  int lvalue = 0;
  int const const_lvalue = 23;
  
  check<0>( 1, lvalue, const_lvalue );
  check<1>( 2, lvalue, const_lvalue );
  check<2>( 3, lvalue, const_lvalue );
  
  check<0>( std::move(const_lvalue) );
  
  auto t = std::make_tuple( 0, std::ref(lvalue) );
  auto && x = etude::get_nth_argument<0>( const_lvalue, etude::unpack(t) );
  BOOST_CHECK( is_explicitly_same_object( x, const_lvalue ) );
  auto && y = etude::get_nth_argument<1>( const_lvalue, etude::unpack(t) );
  BOOST_CHECK( is_explicitly_same_object( y, etude::tuple_get<0>(t) ) );
  auto && z = etude::get_nth_argument<2>( const_lvalue, etude::unpack(t) );
  BOOST_CHECK( is_explicitly_same_object( z, etude::tuple_get<1>(t) ) );
  
  auto && u = etude::get_nth_argument<1>( const_lvalue, etude::unpack( std::move(t) ) );
  BOOST_CHECK(
    is_explicitly_same_object(
      std::forward<decltype(u)>(u), etude::tuple_get<0>( std::move(t) )
    )
  );
  
  return 0;
}
