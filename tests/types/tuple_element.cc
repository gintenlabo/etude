//
//  tuple_element のテストです。
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/types/tuple_element.hpp"
#include <type_traits>

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

// 停止用
template<class Tuple, std::size_t I,
  class = typename std::enable_if<( I >= std::tuple_size<Tuple>::value )>::type
>
void check_( ... ) {}

// チェック実装
template<class Tuple, std::size_t I,
  class = typename std::enable_if<( I < std::tuple_size<Tuple>::value )>::type
>
void check_( int )
{
  typedef typename std::tuple_element<I, Tuple>::type element_type;
  
  // 普通の場合
  STATIC_ASSERT((
    std::is_same< element_type,
      typename etude::tuple_element<I, Tuple>::type
    >::value
  ));
  
  // CV qualifiers
  STATIC_ASSERT((
    std::is_same< element_type const,
      typename etude::tuple_element<I, Tuple const>::type
    >::value
  ));
  STATIC_ASSERT((
    std::is_same< element_type volatile,
      typename etude::tuple_element<I, Tuple volatile>::type
    >::value
  ));
  STATIC_ASSERT((
    std::is_same< element_type const volatile,
      typename etude::tuple_element<I, Tuple const volatile>::type
    >::value
  ));
  
  // references
  STATIC_ASSERT((
    std::is_same< element_type &,
      typename etude::tuple_element<I, Tuple &>::type
    >::value
  ));
  STATIC_ASSERT((
    std::is_same< element_type const &,
      typename etude::tuple_element<I, Tuple const &>::type
    >::value
  ));
  STATIC_ASSERT((
    std::is_same< element_type &&,
      typename etude::tuple_element<I, Tuple &&>::type
    >::value
  ));
  
  return check_<Tuple, I+1>(0);
}

// 本体
template<class Tuple>
void check()
{
  return check_<Tuple, 0>(0);
}

int main()
{
  check< std::tuple<int> >();
  check< std::tuple<int, double> >();
  check< std::tuple<char*, int const&, double&> >();
  check< std::tuple<int, char, int&&, std::tuple<int, double>, char const> >();
}
