//
//  last のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/functional/last.hpp"

#include <type_traits>
#include <boost/utility/addressof.hpp>
#include <boost/assert.hpp>
#include "../../etude/utility/forward_as_tuple.hpp"
#include "../../etude/unpack.hpp"
#include "../../etude/types/last_type.hpp"

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

template<class... Args>
inline void check( Args&&... args )
{
  typedef typename etude::last_type<Args...>::type&& expected_type;
  
  STATIC_ASSERT((
    std::is_same< expected_type,
      decltype(
        etude::last( std::declval<Args>()... )
      )
    >::value
  ));
  
  typedef std::tuple<Args...> tuple_type;
  
  STATIC_ASSERT((
    std::is_same< expected_type,
      decltype(
        etude::last( etude::unpack( std::declval<tuple_type>() ) )
      )
    >::value
  ));
  
  // アドレスチェック
  auto && x1 = etude::last( std::forward<Args>(args)... );
  
  auto t = etude::forward_as_tuple( std::forward<Args>(args)... );
  auto && x2 = etude::last( etude::unpack( std::move(t) ) );
  
  BOOST_ASSERT( boost::addressof(x1) == boost::addressof(x2) );
}

int main()
{
  int lvalue = 0;
  int const const_lvalue = 23;
  
  check( 1, lvalue, const_lvalue );
  check( lvalue, const_lvalue, 2 );
  check( const_lvalue, 3, lvalue );
  check( std::move(const_lvalue) );
  
  // 空 tuple は無理
  // etude::last( etude::unpack( std::make_tuple() ) );
}
