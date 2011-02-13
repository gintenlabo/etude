//
//  storage_size のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/types/storage_align.hpp"

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

int main()
{
  STATIC_ASSERT(( etude::storage_align<void>::value == 0 ));
  STATIC_ASSERT(( etude::storage_align<void const>::value == 0 ));
  STATIC_ASSERT(( etude::storage_align<void ()>::value == 0 ));
  STATIC_ASSERT(( etude::storage_align<void (*)()>::value == alignof(void (*)()) ));
  
  struct func_ref_wrapper{ void (&x)(); };
  STATIC_ASSERT(( etude::storage_align<void(&)()>::value == alignof(func_ref_wrapper) ));
  
  STATIC_ASSERT(( etude::storage_align<int>::value == alignof(int) ));
  STATIC_ASSERT(( etude::storage_align<int*>::value == alignof(int*) ));
  
  struct int_ref_wrapper{ int& x; };
  STATIC_ASSERT(( etude::storage_align<int&>::value == alignof(int_ref_wrapper) ));
  
  STATIC_ASSERT(( etude::storage_align<int[]>::value == alignof(int) ));
  STATIC_ASSERT(( etude::storage_align<int(&)[]>::value == alignof(int_ref_wrapper) ));
  
  STATIC_ASSERT(( etude::storage_align<int[2]>::value == alignof(int) ));
  STATIC_ASSERT(( etude::storage_align<int(&)[2]>::value == alignof(int_ref_wrapper) ));
}
