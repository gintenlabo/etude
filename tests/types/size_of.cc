//
//  size_of のテストです。
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/types/size_of.hpp"

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

int main()
{
  STATIC_ASSERT(( etude::size_of<void>::value == 0 ));
  STATIC_ASSERT(( etude::size_of<void const>::value == 0 ));
  STATIC_ASSERT(( etude::size_of<void ()>::value == 0 ));
  STATIC_ASSERT(( etude::size_of<void (&)()>::value == 0 ));
  STATIC_ASSERT(( etude::size_of<void (*)()>::value == sizeof(void (*)()) ));
  
  STATIC_ASSERT(( etude::size_of<int>::value == sizeof(int) ));
  STATIC_ASSERT(( etude::size_of<int&>::value == sizeof(int) ));
  STATIC_ASSERT(( etude::size_of<int*>::value == sizeof(int*) ));
  
  STATIC_ASSERT(( etude::size_of<int[]>::value == 0 ));
  STATIC_ASSERT(( etude::size_of<int(&)[]>::value == 0 ));
  STATIC_ASSERT(( etude::size_of<int[2]>::value == sizeof(int) * 2 ));
  STATIC_ASSERT(( etude::size_of<int(&)[2]>::value == sizeof(int) * 2 ));
}
