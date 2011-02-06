//
//  null_constant のテストです。
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/types/null_constant.hpp"

#include <type_traits>
#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

template<class T, class = void>
struct has_value_
  : std::false_type {};

template<class T>
struct has_value_< T,
  typename std::enable_if<
    std::is_convertible< decltype( T::value ), typename T::value_type >::value
  >::type
>
  : std::true_type
{
};

template<class T>
struct has_value
  : has_value_<T>::type {};

STATIC_ASSERT(( !has_value<int>::value ));
STATIC_ASSERT(( has_value<std::true_type>::value ));
STATIC_ASSERT(( has_value<std::integral_constant<int, 0>>::value ));


template<class T>
void check()
{
  typedef etude::null_constant<T> tested;
  
  STATIC_ASSERT(( std::is_empty<tested>::value ));
  STATIC_ASSERT(( std::is_trivial<tested>::value ));
  
  STATIC_ASSERT(( std::is_same<typename tested::type, tested>::value ));
  STATIC_ASSERT(( std::is_same<typename tested::value_type, T>::value ));
  
  STATIC_ASSERT(( !has_value<tested>::value ));
}

int main()
{
  check<void>();
  check<int>();
  check<std::size_t>();
  
  STATIC_ASSERT(( std::is_same<etude::null_constant<void>, etude::void_type>::value ));
}
