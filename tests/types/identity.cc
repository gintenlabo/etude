//
//  identity のテストです。
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/types/identity.hpp"

#include <type_traits>
#include <tuple>

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

// etude::identity をチェックする
template<class T>
void check()
{
  // 基本的な性質のみ
  STATIC_ASSERT(( std::is_empty< etude::identity<T> >::value ));
  STATIC_ASSERT(( std::is_trivial< etude::identity<T> >::value ));
  STATIC_ASSERT(( std::is_same< typename etude::identity<T>::type, T >::value ));
}

int main()
{
  // (cv-qualified) void
  check<void>();
  check<void const>();
  
  // basic-types
  check<int>();
  check<double>();
  check<int const>();
  check<char volatile>();
  struct X{};
  check<X>();
  
  // pointers
  check<int*>();
  check<X*>();
  check<double const*>();
  check<int* const>();
  check<char* volatile*>();
  
  // arrays
  check<int[]>();
  check<int[3]>();
  check<double const [][3]>();
  check<char volatile (*)[4][3]>();

  // references
  check<int&>();
  check<double&&>();
  check<int const&>();
  check<char volatile&&>();
  check<char (&)[]>();
  
  // functions
  check<void ()>();
  check<int* (int, int&)>();
  check<int* (*)(int, int&)>();
  check<int* (&)(int, int&)>();
  
  // pointer-to-member
  check<int X::*>();
  check<int* (X::*)(int, int&)>();
  
}
