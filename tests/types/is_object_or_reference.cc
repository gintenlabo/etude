//
//  etude::is_object_or_reference のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/types/is_object_or_reference.hpp"

#include <type_traits>
#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

struct X {};

int main()
{
  // void, 関数
  STATIC_ASSERT(( !etude::is_object_or_reference<void>::value ));
  STATIC_ASSERT(( !etude::is_object_or_reference<void const>::value ));
  STATIC_ASSERT(( !etude::is_object_or_reference<void()>::value ));
  
  // 参照
  STATIC_ASSERT((  etude::is_object_or_reference<int&>::value ));
  STATIC_ASSERT((  etude::is_object_or_reference<int&&>::value ));
  
  // scalar types
  STATIC_ASSERT((  etude::is_object_or_reference<int>::value ));
  STATIC_ASSERT((  etude::is_object_or_reference<int const>::value ));
  STATIC_ASSERT((  etude::is_object_or_reference<double>::value ));
  STATIC_ASSERT((  etude::is_object_or_reference<int*>::value ));
  STATIC_ASSERT((  etude::is_object_or_reference<void(*)()>::value ));
  STATIC_ASSERT((  etude::is_object_or_reference<int X::*>::value ));
  STATIC_ASSERT((  etude::is_object_or_reference<void (X::*)()>::value ));
  
  // ユーザ定義型
  STATIC_ASSERT((  etude::is_object_or_reference<X>::value ));
  STATIC_ASSERT((  etude::is_object_or_reference<X const>::value ));
  
}
