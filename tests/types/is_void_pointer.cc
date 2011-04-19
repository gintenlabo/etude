//
//  etude::is_void_pointer のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/types/is_void_pointer.hpp"

#include <type_traits>

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

int main()
{
  STATIC_ASSERT(( !etude::is_void_pointer<void>::value ));
  STATIC_ASSERT(( !etude::is_void_pointer<int>::value ));
  STATIC_ASSERT(( !etude::is_void_pointer<int*>::value ));
  STATIC_ASSERT(( !etude::is_void_pointer<int**>::value ));
  STATIC_ASSERT(( !etude::is_void_pointer<void**>::value ));
  STATIC_ASSERT(( !etude::is_void_pointer<void*&>::value ));
  STATIC_ASSERT(( !etude::is_void_pointer<void*&&>::value ));
  
  
  STATIC_ASSERT(( etude::is_void_pointer<void*>::value ));
  STATIC_ASSERT(( etude::is_void_pointer<void* const>::value ));
  STATIC_ASSERT(( etude::is_void_pointer<void* volatile>::value ));
  STATIC_ASSERT(( etude::is_void_pointer<void* const volatile>::value ));
  
  STATIC_ASSERT(( etude::is_void_pointer<void const*>::value ));
  STATIC_ASSERT(( etude::is_void_pointer<void const* const>::value ));
  STATIC_ASSERT(( etude::is_void_pointer<void const* volatile>::value ));
  STATIC_ASSERT(( etude::is_void_pointer<void const* const volatile>::value ));
  
  STATIC_ASSERT(( etude::is_void_pointer<void volatile*>::value ));
  STATIC_ASSERT(( etude::is_void_pointer<void volatile* const>::value ));
  STATIC_ASSERT(( etude::is_void_pointer<void volatile* volatile>::value ));
  STATIC_ASSERT(( etude::is_void_pointer<void volatile* const volatile>::value ));
  
  STATIC_ASSERT(( etude::is_void_pointer<void const volatile*>::value ));
  STATIC_ASSERT(( etude::is_void_pointer<void const volatile* const>::value ));
  STATIC_ASSERT(( etude::is_void_pointer<void const volatile* volatile>::value ));
  STATIC_ASSERT(( etude::is_void_pointer<void const volatile* const volatile>::value ));
  
}
