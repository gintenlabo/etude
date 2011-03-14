//
//  etude::has_common_type のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/types/has_common_type.hpp"

#include <type_traits>

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

struct Base {};
struct Derived : Base {};

int main()
{
  // 引数のない場合
  STATIC_ASSERT(( !etude::has_common_type<>::value ));
  
  // １引数
  STATIC_ASSERT((  etude::has_common_type<void>::value ));
  STATIC_ASSERT((  etude::has_common_type<int>::value ));
  
  // ２引数
  STATIC_ASSERT((  etude::has_common_type<void, void>::value ));
  STATIC_ASSERT(( !etude::has_common_type<int, void>::value ));
  STATIC_ASSERT(( !etude::has_common_type<void, int>::value ));
  STATIC_ASSERT((  etude::has_common_type<int, int>::value ));
  STATIC_ASSERT((  etude::has_common_type<int, double>::value ));
  STATIC_ASSERT(( !etude::has_common_type<int, int*>::value ));
  STATIC_ASSERT((  etude::has_common_type<Base volatile*, Derived const*>::value ));
  STATIC_ASSERT(( !etude::has_common_type<int*, char*>::value ));
  STATIC_ASSERT(( !etude::has_common_type<Base**, Derived**>::value ));
  
  // ３引数以上
  STATIC_ASSERT((  etude::has_common_type<void, void, void>::value ));
  STATIC_ASSERT(( !etude::has_common_type< int, void, void>::value ));
  STATIC_ASSERT(( !etude::has_common_type<void,  int, void>::value ));
  STATIC_ASSERT(( !etude::has_common_type<void, void,  int>::value ));
  STATIC_ASSERT(( !etude::has_common_type<void,  int,  int>::value ));
  STATIC_ASSERT(( !etude::has_common_type< int, void,  int>::value ));
  STATIC_ASSERT(( !etude::has_common_type< int,  int, void>::value ));
  STATIC_ASSERT((  etude::has_common_type< int,  int,  int>::value ));
  
  STATIC_ASSERT((  etude::has_common_type<int, char, double>::value ));
  STATIC_ASSERT((  etude::has_common_type<int const*, void*, char*>::value ));
  STATIC_ASSERT(( !etude::has_common_type<int const*, char*, void*>::value ));
  
  STATIC_ASSERT((
    etude::has_common_type<void*, int const*, double volatile*, Base*>::value
  ));
}
