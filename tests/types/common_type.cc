//
//  etude::common_type のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/types/common_type.hpp"

#include <type_traits>
#include "../../etude/types/get_type_or.hpp"

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

template<class Expected, class... Types>
inline void check()
{
  STATIC_ASSERT(( etude::has_common_type<Types...>::value ));
  
  STATIC_ASSERT ((
    std::is_same<
      typename etude::common_type<Types...>::type, Expected
    >::value
  ));
  
  STATIC_ASSERT ((
    std::is_same<
      typename std::common_type<Types...>::type, Expected
    >::value
  ));
  
}

template<class... Types>
inline void check_not_defined()
{
  STATIC_ASSERT(( !etude::has_common_type<Types...>::value ));
  
  class X {};
  STATIC_ASSERT((
    std::is_same< X,
      typename etude::get_type_or<etude::common_type<Types...>, X>::type
    >::value
  ));
}


struct Base {};
struct Derived : Base {};

int main()
{
  // 引数のない場合
  check_not_defined<>();
  
  // １引数
  check<void, void>();
  check<int, int>();
  
  // ２引数
  check<void, void, void>();
  check<int, int, int>();
  check_not_defined<void, int>();
  
  check<Base const volatile*, Base volatile*, Derived const*>();
  check_not_defined<int*, char*>();
  check_not_defined<Base**, Derived**>();
  
  // ３引数以上
  check<void, void, void, void>();
  check<int, int, int, int>();
  check_not_defined<int, void, int>();
  
  check<double, int, char, double>();
  check<void const*, int const*, void*, char*>();
  check_not_defined<int const*, char*, void*>();
  
  check<void const volatile*, void*, int const*, double volatile*, Base*>();
}
