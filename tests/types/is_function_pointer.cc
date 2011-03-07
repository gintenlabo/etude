//
//  is_function_pointer のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/types/is_function_pointer.hpp"

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

int main()
{
  // 関数とは無関係な型
  STATIC_ASSERT(( !etude::is_function_pointer<void>::value ));
  STATIC_ASSERT(( !etude::is_function_pointer<void const>::value ));
  
  STATIC_ASSERT(( !etude::is_function_pointer<int>::value ));
  STATIC_ASSERT(( !etude::is_function_pointer<int const>::value ));
  STATIC_ASSERT(( !etude::is_function_pointer<int*>::value ));
  STATIC_ASSERT(( !etude::is_function_pointer<int&>::value ));
  STATIC_ASSERT(( !etude::is_function_pointer<int&&>::value ));
  
  class klass {};
  STATIC_ASSERT(( !etude::is_function_pointer<klass>::value ));
  STATIC_ASSERT(( !etude::is_function_pointer<klass const>::value ));
  STATIC_ASSERT(( !etude::is_function_pointer<klass*>::value ));
  STATIC_ASSERT(( !etude::is_function_pointer<klass&>::value ));
  STATIC_ASSERT(( !etude::is_function_pointer<klass&&>::value ));
  
  // 関数
  STATIC_ASSERT(( !etude::is_function_pointer<void()>::value ));
  STATIC_ASSERT(( !etude::is_function_pointer<int ()>::value ));
  STATIC_ASSERT(( !etude::is_function_pointer<void(int)>::value ));
  STATIC_ASSERT(( !etude::is_function_pointer<int (int)>::value ));
  STATIC_ASSERT(( !etude::is_function_pointer<int&(int, int*, klass const&)>::value ));
  
  // 関数ポインタ
  STATIC_ASSERT(( etude::is_function_pointer<void(*)()>::value ));
  STATIC_ASSERT(( etude::is_function_pointer<int (*)()>::value ));
  STATIC_ASSERT(( etude::is_function_pointer<void(*)(int)>::value ));
  STATIC_ASSERT(( etude::is_function_pointer<int (*)(int)>::value ));
  STATIC_ASSERT(( etude::is_function_pointer<int&(*)(int, int*, klass const&)>::value ));
  
  STATIC_ASSERT(( etude::is_function_pointer<void(*)()>::value ));
  STATIC_ASSERT(( etude::is_function_pointer<void(* const)()>::value ));
  STATIC_ASSERT(( etude::is_function_pointer<void(* volatile)()>::value ));
  STATIC_ASSERT(( etude::is_function_pointer<void(* const volatile)()>::value ));
  
  // 関数への参照
  STATIC_ASSERT(( !etude::is_function_pointer<void(&)()>::value ));
  STATIC_ASSERT(( !etude::is_function_pointer<int (&)()>::value ));
  STATIC_ASSERT(( !etude::is_function_pointer<void(&)(int)>::value ));
  STATIC_ASSERT(( !etude::is_function_pointer<int (&)(int)>::value ));
  STATIC_ASSERT(( !etude::is_function_pointer<int&(&)(int, int*, klass const&)>::value ));
  
  STATIC_ASSERT(( !etude::is_function_pointer<void(&&)()>::value ));
  
  // メンバ関数ポインタ（関数ポインタではない）
  STATIC_ASSERT(( !etude::is_function_pointer<void (klass::*)()>::value ));
  STATIC_ASSERT(( !etude::is_function_pointer<int  (klass::*)()>::value ));
  STATIC_ASSERT(( !etude::is_function_pointer<void (klass::*)(int)>::value ));
  STATIC_ASSERT(( !etude::is_function_pointer<int  (klass::*)(int)>::value ));
  STATIC_ASSERT(( !etude::is_function_pointer<int  (klass::* const)(int)>::value ));
  STATIC_ASSERT(( !etude::is_function_pointer<int  (klass::*)(int) const>::value ));
  
  // メンバ変数ポインタ（同じく関数ポインタではない）
  STATIC_ASSERT(( !etude::is_function_pointer<int klass::*>::value ));
  STATIC_ASSERT(( !etude::is_function_pointer<const int klass::*>::value ));
  STATIC_ASSERT(( !etude::is_function_pointer<int klass::* const>::value ));
}
