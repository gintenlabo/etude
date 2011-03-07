//
//  function_base のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#include "../../etude/functional/function_base.hpp"

#include <type_traits>
#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

// 引数なしの場合
struct tested0
  : etude::function_base<void(), tested0> {};
STATIC_ASSERT(( std::is_same< tested0::result_type, void >::value ));

// 一引数の場合
struct tested1
  : etude::function_base<double(int), tested1> {};
STATIC_ASSERT(( std::is_same< tested1::result_type, double >::value ));
STATIC_ASSERT(( std::is_same< tested1::argument_type, int >::value ));

// 二引数の場合
struct tested2
  : etude::function_base<int(int&, void*), tested2> {};
STATIC_ASSERT(( std::is_same< tested2::result_type, int >::value ));
STATIC_ASSERT(( std::is_same< tested2::first_argument_type, int& >::value ));
STATIC_ASSERT(( std::is_same< tested2::second_argument_type, void* >::value ));

// それ以上の場合
struct tested3
  : etude::function_base<void(char const*, void*&, int), tested3> {};
STATIC_ASSERT(( std::is_same< tested3::result_type, void >::value ));

int main()
{
  // タグなしも一応チェック
  typedef etude::function_base<int()> base0;
  STATIC_ASSERT(( std::is_same< base0::result_type, int >::value ));
  
  typedef etude::function_base<void(int)> base1;
  STATIC_ASSERT(( std::is_same< base1::result_type, void >::value ));
  STATIC_ASSERT(( std::is_same< base1::argument_type, int >::value ));
  
  typedef etude::function_base<void(char*, long)> base2;
  STATIC_ASSERT(( std::is_same< base2::result_type, void >::value ));
  STATIC_ASSERT(( std::is_same< base2::first_argument_type, char* >::value ));
  STATIC_ASSERT(( std::is_same< base2::second_argument_type, long >::value ));
  
  typedef etude::function_base<int(char const*, ...)> baseX;
  STATIC_ASSERT(( std::is_same< baseX::result_type, int >::value ));
}
