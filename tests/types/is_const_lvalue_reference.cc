//
//  etude::is_const_lvalue_reference のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/types/is_const_lvalue_reference.hpp"

#include <type_traits>
#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

int main()
{
  // 参照以外
  STATIC_ASSERT(( !etude::is_const_lvalue_reference<void>::value ));
  STATIC_ASSERT(( !etude::is_const_lvalue_reference<void const>::value ));
  STATIC_ASSERT(( !etude::is_const_lvalue_reference<void()>::value ));
  STATIC_ASSERT(( !etude::is_const_lvalue_reference<int>::value ));
  STATIC_ASSERT(( !etude::is_const_lvalue_reference<int const>::value ));
  STATIC_ASSERT(( !etude::is_const_lvalue_reference<int*>::value ));
  
  // lvalue reference
  STATIC_ASSERT(( !etude::is_const_lvalue_reference<int &>::value ));
  STATIC_ASSERT((  etude::is_const_lvalue_reference<int const&>::value ));
  STATIC_ASSERT(( !etude::is_const_lvalue_reference<int volatile&>::value ));
  STATIC_ASSERT(( !etude::is_const_lvalue_reference<int const volatile&>::value ));
  
  // rvalue reference
  STATIC_ASSERT(( !etude::is_const_lvalue_reference<int &&>::value ));
  STATIC_ASSERT(( !etude::is_const_lvalue_reference<int const&&>::value ));
  STATIC_ASSERT(( !etude::is_const_lvalue_reference<int volatile&&>::value ));
  STATIC_ASSERT(( !etude::is_const_lvalue_reference<int const volatile&&>::value ));

}
