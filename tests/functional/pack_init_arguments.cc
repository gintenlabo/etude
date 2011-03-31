//
//  pack_init_arguments のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/functional/pack_init_arguments.hpp"

#include <type_traits>
#include <boost/utility/addressof.hpp>
#include <boost/test/minimal.hpp>
#include "../../etude/utility/forward_as_tuple.hpp"
#include "../../etude/utility/tuple_init.hpp"
#include "../../etude/unpack.hpp"
#include "../../etude/functional/pack.hpp"
#include "../../etude/functional/group.hpp"

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

template<class T, class U>
bool is_explicitly_same_object( T && x, U && y )
{
  return std::is_same<T, U>::value && ( boost::addressof(x) == boost::addressof(y) );
}

template< class... Args >
inline void check( Args&&... args )
{
  typedef decltype( etude::tuple_init( etude::pack( std::declval<Args>()... ) ) ) pack_t;
      
  STATIC_ASSERT((
    std::is_same< pack_t,
      decltype(
        etude::pack_init_arguments( std::declval<Args>()... )
      )
    >::value
  ));
  
  typedef std::tuple<Args...> tuple_type;
  
  STATIC_ASSERT((
    std::is_same< pack_t,
      decltype(
        etude::pack_init_arguments( etude::unpack( std::declval<tuple_type>() ) )
      )
    >::value
  ));
  
  // アドレスチェックは省略
  auto t = etude::forward_as_tuple( std::forward<Args>(args)... );
  auto p = etude::pack_init_arguments( std::forward<Args>(args)... );
  (void)t, (void)p;
}

int test_main( int, char** )
{
  int lvalue = 0;
  int const const_lvalue = 23;
  
  check( 1, lvalue, const_lvalue );
  check( lvalue, const_lvalue, 2 );
  check( const_lvalue, 3, lvalue );
  check( std::move(const_lvalue) );
  
  // 拡張 unpack
  auto t = std::make_tuple( 0, std::ref(lvalue) );
  auto p = etude::pack_init_arguments( const_lvalue, etude::unpack(t) );
  
  BOOST_CHECK( is_explicitly_same_object( std::get<0>(p), const_lvalue ) );
  BOOST_CHECK( is_explicitly_same_object( std::get<1>(p), std::get<0>(t) ) );
  
  return 0;
}