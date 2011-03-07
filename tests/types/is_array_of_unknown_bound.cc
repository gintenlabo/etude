//
//  is_array_of_unknown_bound のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/types/is_array_of_unknown_bound.hpp"
\
#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

int main()
{
  STATIC_ASSERT(( etude::is_array_of_unknown_bound<void>::value == false ));
  STATIC_ASSERT(( etude::is_array_of_unknown_bound<void()>::value == false ));
  STATIC_ASSERT(( etude::is_array_of_unknown_bound<int&>::value == false ));
  STATIC_ASSERT(( etude::is_array_of_unknown_bound<int>::value == false ));
  STATIC_ASSERT(( etude::is_array_of_unknown_bound<int*>::value == false ));
  
  STATIC_ASSERT(( etude::is_array_of_unknown_bound<int[]>::value == true ));
  STATIC_ASSERT(( etude::is_array_of_unknown_bound<int[2]>::value == false ));
  STATIC_ASSERT(( etude::is_array_of_unknown_bound<int[][2]>::value == true ));
  STATIC_ASSERT(( etude::is_array_of_unknown_bound<int[2][2]>::value == false ));
  
  STATIC_ASSERT(( etude::is_array_of_unknown_bound<int(&)[]>::value == false ));
  STATIC_ASSERT(( etude::is_array_of_unknown_bound<int * []>::value == true ));
  STATIC_ASSERT(( etude::is_array_of_unknown_bound<int(*)[]>::value == false ));
}
