//
//  size_of のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/types/size_of.hpp"

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

template<class T, std::size_t N>
void check()
{
  STATIC_ASSERT(( N == sizeof(T) ));
  
  STATIC_ASSERT(( etude::size_of<T>::is_defined ));
  STATIC_ASSERT(( etude::size_of<T>::value == N ));
}

template<class T>
void check_not_defined() {
  STATIC_ASSERT(( !etude::size_of<T>::is_defined ));
  STATIC_ASSERT(( etude::size_of<T>::value == 0 ));
}

int main()
{
  check_not_defined<void>();
  check_not_defined<void const>();
  check_not_defined<void    ()>();
  check_not_defined<void (&)()>();
  check<void (*)(), sizeof(void (*)())>();
  
  check<int,  sizeof(int)>();
  check<int&, sizeof(int)>();
  check<int*, sizeof(int*)>();
  
  check_not_defined<int   []>();
  check_not_defined<int(&)[]>();
  check<int   [2], sizeof(int)*2>();
  check<int(&)[2], sizeof(int)*2>();
  check_not_defined<int   [][2]>();
  check_not_defined<int(&)[][2]>();
  check<int   [2][2], sizeof(int)*2*2>();
  check<int(&)[2][2], sizeof(int)*2*2>();
}
