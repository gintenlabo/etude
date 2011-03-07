//
//  get_nth のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/utility/get_nth.hpp"

#include <type_traits>
#include <boost/utility/addressof.hpp>
#include <boost/assert.hpp>
#include "../../etude/utility/forward_as_tuple.hpp"
#include "../../etude/unpack.hpp"
#include "../../etude/types/nth_type.hpp"

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

template<std::size_t N, class... Args>
inline void check( Args&&... args )
{
  typedef typename etude::nth_type<N, Args...>::type&& expected_type;
  
  STATIC_ASSERT((
    std::is_same< expected_type,
      decltype(
        etude::get_nth<N>( std::declval<Args>()... )
      )
    >::value
  ));
  
  typedef std::tuple<Args...> tuple_type;
  
  STATIC_ASSERT((
    std::is_same< expected_type,
      decltype(
        etude::get_nth<N>( etude::unpack( std::declval<tuple_type>() ) )
      )
    >::value
  ));
  
  // アドレスチェック
  auto && x1 = etude::get_nth<N>( std::forward<Args>(args)... );
  
  auto t = etude::forward_as_tuple( std::forward<Args>(args)... );
  auto && x2 = etude::get_nth<N>( etude::unpack( std::move(t) ) );
  
  BOOST_ASSERT( boost::addressof(x1) == boost::addressof(x2) );
}

int main()
{
  int lvalue = 0;
  int const const_lvalue = 23;
  
  check<0>( 1, lvalue, const_lvalue );
  check<1>( 2, lvalue, const_lvalue );
  check<2>( 3, lvalue, const_lvalue );
  
  check<0>( std::move(const_lvalue) );
}
