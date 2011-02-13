//
//  make_void のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/types/make_void.hpp"

#include <type_traits>

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

// etude::make_void をチェックする
template<class... Types>
void check()
{
  typedef etude::make_void<Types...> tested;
  
  // 基本的な性質のみ
  STATIC_ASSERT(( std::is_empty<tested>::value ));
  STATIC_ASSERT(( std::is_trivial<tested>::value ));
  STATIC_ASSERT(( std::is_same< typename tested::type, void >::value ));
}

int main()
{
  struct X{};
  
  check<void>();
  check<void const>();
  check<int, double>();
  check<int const, char volatile, X, X&, int*>();
  check<int&, double&&, char (&)[], void ()>();
  check<int* (&)(int, int&), int X::*, int* (X::*)(int, int&)>();
  
}
