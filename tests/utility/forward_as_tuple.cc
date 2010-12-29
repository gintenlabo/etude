//
//  forward_as_tuple のテストです。
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/utility/forward_as_tuple.hpp"

#include <type_traits>
#include <utility>

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

template<class... Args>
inline std::tuple<Args&&...> check( Args&&... args )
{
  STATIC_ASSERT((
    std::is_same<
      decltype( etude::forward_as_tuple( std::forward<Args>(args)... ) ),
      std::tuple<Args&&...>
    >::value
  ));
  // 実際に呼び出す
  return etude::forward_as_tuple( std::forward<Args>(args)... );
}

#include <boost/assert.hpp>

int main()
{
  {
    auto t = check();
    
    STATIC_ASSERT((
      std::is_same<
        decltype(t),
        std::tuple<>
      >::value
    ));
  }
  
  {
    int lvalue;
    int const const_lvalue = 0;
    
    auto t = check( 0, lvalue, const_lvalue );
    
    STATIC_ASSERT((
      std::is_same<
        decltype(t),
        std::tuple<int&&, int&, int const&>
      >::value
    ));
  }
  
  {
    std::tuple<char const*> t = check( "string literal" );
    BOOST_ASSERT( std::get<0>(t)[0] == 's' );
  }
}
