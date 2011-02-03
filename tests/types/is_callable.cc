//
//  is_callable のテストです。
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/types/is_callable.hpp"

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

struct twice {
  template<class T,
    class = decltype( std::declval<T const&>() + std::declval<T const&>() )
  >
  T operator()( T const& x ) const {
    return x + x;
  }
};

struct counter
{
  explicit counter( int initial_count = 0 )
    : count_(initial_count) {}
  
  inline int operator()() {
    return ++count_;
  }
  
 private:
  int count_;
};

int main()
{
  // 引数なしの場合
  // 型変換なし
  STATIC_ASSERT(( etude::is_callable<void (*)(), void ()>::value ));
  STATIC_ASSERT(( etude::is_callable<void (* const)(), void ()>::value ));
  STATIC_ASSERT(( etude::is_callable<void (&)(), void ()>::value ));
  STATIC_ASSERT(( etude::is_callable<void (&&)(), void ()>::value ));
  
  // 型変換有り
  STATIC_ASSERT((  etude::is_callable<int (*)(), void ()>::value ));
  STATIC_ASSERT(( !etude::is_callable<void (*)(), int ()>::value ));
  STATIC_ASSERT((  etude::is_callable<int (*)(), double ()>::value ));
  STATIC_ASSERT((  etude::is_callable<int (*)(), bool ()>::value ));
  STATIC_ASSERT(( !etude::is_callable<int (*)(), void* ()>::value ));
  
  // 引数あり
  STATIC_ASSERT((  etude::is_callable<int (*)(int), int (int)>::value ));
  STATIC_ASSERT(( !etude::is_callable<int (*)(), int (int)>::value ));
  STATIC_ASSERT(( !etude::is_callable<int (*)(void*), int (int)>::value ));
  STATIC_ASSERT((  etude::is_callable<int (*)(int), bool (double)>::value ));
  STATIC_ASSERT(( !etude::is_callable<void (*)(int), void (int, int)>::value ));
  STATIC_ASSERT(( !etude::is_callable<void (*)(int, int), void (int)>::value ));
  
  // 関数オブジェクト
  STATIC_ASSERT((  etude::is_callable<twice, int (int)>::value ));
  STATIC_ASSERT((  etude::is_callable<twice const, int (int)>::value ));
  STATIC_ASSERT((  etude::is_callable<twice, void (int)>::value ));
  STATIC_ASSERT(( !etude::is_callable<twice, void (void*)>::value ));
  STATIC_ASSERT(( !etude::is_callable<twice, void ()>::value ));
  STATIC_ASSERT(( !etude::is_callable<twice, void (int, int)>::value ));
  
  STATIC_ASSERT((  etude::is_callable<counter, int ()>::value ));
  STATIC_ASSERT(( !etude::is_callable<counter const, int ()>::value ));
  
  // メンバポインタ
  struct X
  {
    int member;
    void f(){}
    int g( void*, double ) const { return member; }
  };
  STATIC_ASSERT((  etude::is_callable<decltype(&X::member), int& (X&)>::value ));
  STATIC_ASSERT(( !etude::is_callable<decltype(&X::member), int& (X const&)>::value ));
  STATIC_ASSERT((  etude::is_callable<decltype(&X::member), int  (X const&)>::value ));
  STATIC_ASSERT((  etude::is_callable<decltype(&X::f), void (X&)>::value ));
  STATIC_ASSERT(( !etude::is_callable<decltype(&X::f), int  (X&)>::value ));
  STATIC_ASSERT((  etude::is_callable<decltype(&X::g), int ( X const&, void*, double )>::value ));
  STATIC_ASSERT((  etude::is_callable<decltype(&X::g), void ( X&, int*, float )>::value ));
  STATIC_ASSERT(( !etude::is_callable<decltype(&X::g), void ()>::value ));
  STATIC_ASSERT(( !etude::is_callable<decltype(&X::g), void ( X& )>::value ));
  STATIC_ASSERT(( !etude::is_callable<decltype(&X::g), void ( X&, int, double )>::value ));
}
