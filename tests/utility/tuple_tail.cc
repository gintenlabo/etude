//
//  tuple_tail のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/utility/tuple_tail.hpp"

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

#include <type_traits>
#include <utility>
#include <tuple>
#include <array>

int main()
{
  // ill-formed
  // etude::tuple_tail( std::make_tuple() );
  
  {
    int i = 0;
    std::tuple<int, int&, int const&, int&&> p( 23, i, i, std::move(i) );
    typedef std::tuple<int&, int const&, int&&> tuple_type;
    
    STATIC_ASSERT((
      std::is_same< decltype( etude::tuple_tail(p) ), tuple_type >::value
    ));
    
    auto t = etude::tuple_tail( std::move(p) );
    STATIC_ASSERT((
      std::is_same< decltype(t), tuple_type >::value
    ));
  }
  
  {
    std::pair<int, char const*> p( 23, "hoge" );
    typedef std::tuple<char const*> tuple_type;
    
    STATIC_ASSERT((
      std::is_same< decltype( etude::tuple_tail(p) ), tuple_type >::value
    ));
    
    auto t = etude::tuple_tail( std::move(p) );
    STATIC_ASSERT((
      std::is_same< decltype(t), tuple_type >::value
    ));
  }
  
  {
    std::array<int const, 3> a = {{ 1, 2, 3 }};
    typedef std::tuple<int const, int const> tuple_type;
    
    STATIC_ASSERT((
      std::is_same< decltype( etude::tuple_tail(a) ), tuple_type >::value
    ));
    
    auto t = etude::tuple_tail( std::move(a) );
    STATIC_ASSERT((
      std::is_same< decltype(t), tuple_type >::value
    ));
  }
}
