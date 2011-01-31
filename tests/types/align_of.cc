//
//  align_of のテストです。
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/types/align_of.hpp"

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

int main()
{
  STATIC_ASSERT(( etude::align_of<void>::value == 0 ));
  STATIC_ASSERT(( etude::align_of<void const>::value == 0 ));
  STATIC_ASSERT(( etude::align_of<void ()>::value == 0 ));
  STATIC_ASSERT(( etude::align_of<void (&)()>::value == 0 ));
  STATIC_ASSERT(( etude::align_of<void (*)()>::value == alignof(void (*)()) ));
  
  STATIC_ASSERT(( etude::align_of<int>::value == alignof(int) ));
  STATIC_ASSERT(( etude::align_of<int&>::value == alignof(int) ));
  STATIC_ASSERT(( etude::align_of<int*>::value == alignof(int*) ));
  
  STATIC_ASSERT(( etude::align_of<int[]>::value == alignof(int) ));
  STATIC_ASSERT(( etude::align_of<int(&)[]>::value == alignof(int) ));
  STATIC_ASSERT(( etude::align_of<int[2]>::value == alignof(int) ));
  STATIC_ASSERT(( etude::align_of<int(&)[2]>::value == alignof(int) ));
}
