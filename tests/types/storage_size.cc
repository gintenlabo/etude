//
//  storage_size のテストです。
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/types/storage_size.hpp"

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

int main()
{
  STATIC_ASSERT(( etude::storage_size<void>::value == 0 ));
  STATIC_ASSERT(( etude::storage_size<void const>::value == 0 ));
  STATIC_ASSERT(( etude::storage_size<void ()>::value == 0 ));
  STATIC_ASSERT(( etude::storage_size<void (*)()>::value == sizeof(void (*)()) ));
  
  struct func_ref_wrapper{ void (&x)(); };
  STATIC_ASSERT(( etude::storage_size<void (&)()>::value == sizeof(func_ref_wrapper) ));
  
  STATIC_ASSERT(( etude::storage_size<int>::value == sizeof(int) ));
  STATIC_ASSERT(( etude::storage_size<int*>::value == sizeof(int*) ));
  
  struct int_ref_wrapper{ int& x; };
  STATIC_ASSERT(( etude::storage_size<int&>::value == sizeof(int_ref_wrapper) ));
  
  STATIC_ASSERT(( etude::storage_size<int[]>::value == 0 ));
  STATIC_ASSERT(( etude::storage_size<int(&)[]>::value == sizeof(int_ref_wrapper) ));
  
  STATIC_ASSERT(( etude::storage_size<int[2]>::value == sizeof(int) * 2 ));
  STATIC_ASSERT(( etude::storage_size<int(&)[2]>::value == sizeof(int_ref_wrapper) ));
}
