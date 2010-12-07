//
//  decay_and_strip のテストです。
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/types/decay_and_strip.hpp"

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

template<class T, class Result>
struct check_decay_and_strip
  : std::is_same<typename etude::decay_and_strip<T>::type, Result> {};

void f() {}

int main()
{
  // value
  STATIC_ASSERT(( check_decay_and_strip<int, int>::value ));
  
  // const value
  STATIC_ASSERT(( check_decay_and_strip<int const, int>::value ));
  
  // reference
  STATIC_ASSERT(( check_decay_and_strip<int&, int>::value ));
  
  // const reference
  STATIC_ASSERT(( check_decay_and_strip<int const&, int>::value ));
  
  // array
  int a1[10];
  int a2[2][3];
  STATIC_ASSERT(( check_decay_and_strip<decltype(a1), int*>::value ));
  STATIC_ASSERT(( check_decay_and_strip<decltype(a2), int(*)[3]>::value ));
  
  // function
  STATIC_ASSERT(( check_decay_and_strip<decltype(f), void(*)()>::value ));
  
  // reference_wrapper
  int i;
  STATIC_ASSERT(( check_decay_and_strip<decltype(std::ref(i)), int&>::value ));
  STATIC_ASSERT(( check_decay_and_strip<decltype(std::cref(i)), int const&>::value ));
  
  // reference to reference_wrapper
  STATIC_ASSERT(( check_decay_and_strip<std::reference_wrapper<int> const&, int&>::value ));
  STATIC_ASSERT(( check_decay_and_strip<std::reference_wrapper<int const>&, int const&>::value ));
}
