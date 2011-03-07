//
//  align_of のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/types/align_of.hpp"

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

template<class T, std::size_t N>
void check() {
  STATIC_ASSERT(( etude::align_of<T>::is_defined ));
  STATIC_ASSERT(( etude::align_of<T>::value == N ));
}

template<class T>
void check_not_defined() {
  STATIC_ASSERT(( !etude::align_of<T>::is_defined ));
  STATIC_ASSERT(( etude::align_of<T>::value == 0 ));
}

int main()
{
  check_not_defined<void>();
  check_not_defined<void const>();
  check_not_defined<void    ()>();
  check_not_defined<void (&)()>();
  check<void (*)(), alignof(void (*)())>();
  
  check<int,  alignof(int)>();
  check<int&, alignof(int)>();
  check<int*, alignof(int*)>();
  
  check<int   [], alignof(int)>();
  check<int(&)[], alignof(int)>();
  check<int   [2], alignof(int)>();
  check<int(&)[2], alignof(int)>();
  check<int   [][2], alignof(int)>();
  check<int(&)[][2], alignof(int)>();
  check<int   [2][2], alignof(int)>();
  check<int(&)[2][2], alignof(int)>();
}
