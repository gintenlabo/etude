//
//  etude::is_constructible のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/types/is_constructible.hpp"

#include <type_traits>
#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

struct Base {};
struct Derived : Base {};

struct X
{
  explicit X( char const* );
  X( char const*, std::size_t );
};

int main()
{
  // void, 関数
  STATIC_ASSERT(( !etude::is_constructible<void>::value ));
  STATIC_ASSERT(( !etude::is_constructible<void const>::value ));
  STATIC_ASSERT(( !etude::is_constructible<void, int>::value ));
  STATIC_ASSERT(( !etude::is_constructible<void, void>::value ));
  STATIC_ASSERT(( !etude::is_constructible<void, int, char>::value ));
  STATIC_ASSERT(( !etude::is_constructible<void()>::value ));
  STATIC_ASSERT(( !etude::is_constructible<void(), int>::value ));
  STATIC_ASSERT(( !etude::is_constructible<void(), double, int*>::value ));
  
  // arithmetic types
  STATIC_ASSERT((  etude::is_constructible<int>::value ));
  STATIC_ASSERT((  etude::is_constructible<int const>::value ));
  STATIC_ASSERT((  etude::is_constructible<int, int>::value ));
  STATIC_ASSERT((  etude::is_constructible<int, char>::value ));
  STATIC_ASSERT(( !etude::is_constructible<int, void>::value ));
  STATIC_ASSERT(( !etude::is_constructible<int, void*>::value ));
  STATIC_ASSERT(( !etude::is_constructible<int, int, int>::value ));
  STATIC_ASSERT((  etude::is_constructible<double>::value ));
  STATIC_ASSERT((  etude::is_constructible<double, int>::value ));
  STATIC_ASSERT((  etude::is_constructible<double, double>::value ));
  STATIC_ASSERT(( !etude::is_constructible<double, void>::value ));
  STATIC_ASSERT(( !etude::is_constructible<double, double*>::value ));
  STATIC_ASSERT(( !etude::is_constructible<double, int, int>::value ));
  
  // ポインタ
  STATIC_ASSERT((  etude::is_constructible<Base*>::value ));
  STATIC_ASSERT((  etude::is_constructible<Base const*>::value ));
  STATIC_ASSERT((  etude::is_constructible<Base* const>::value ));
  STATIC_ASSERT((  etude::is_constructible<void*, Base*>::value ));
  STATIC_ASSERT(( !etude::is_constructible<Base*, void*>::value ));
  STATIC_ASSERT((  etude::is_constructible<Base*, Derived*>::value ));
  STATIC_ASSERT(( !etude::is_constructible<Derived*, Base*>::value ));
  STATIC_ASSERT(( !etude::is_constructible<Base**, Derived**>::value ));
  STATIC_ASSERT(( !etude::is_constructible<Base*, Derived*, int>::value ));

  // 参照
  STATIC_ASSERT(( !etude::is_constructible<int&>::value ));
  STATIC_ASSERT(( !etude::is_constructible<int&&>::value ));
  STATIC_ASSERT((  etude::is_constructible<int&, int&>::value ));
  STATIC_ASSERT(( !etude::is_constructible<int&&, int&>::value ));
  STATIC_ASSERT((  etude::is_constructible<int&&, char&>::value ));
  STATIC_ASSERT(( !etude::is_constructible<Base&>::value ));
  STATIC_ASSERT((  etude::is_constructible<Base&, Derived&>::value ));
  STATIC_ASSERT(( !etude::is_constructible<Base&, Derived const&>::value ));
  STATIC_ASSERT((  etude::is_constructible<Base const&, Derived&>::value ));
  STATIC_ASSERT(( !etude::is_constructible<Base const&, Derived&, int>::value ));
  
  // ユーザ定義型
  STATIC_ASSERT(( !etude::is_constructible<X>::value ));
  STATIC_ASSERT(( !etude::is_constructible<X const>::value ));
  STATIC_ASSERT((  etude::is_constructible<X, char*>::value ));
  STATIC_ASSERT(( !etude::is_constructible<X, void*>::value ));
  STATIC_ASSERT((  etude::is_constructible<X, char*, int>::value ));
  STATIC_ASSERT(( !etude::is_constructible<X, void*, int>::value ));
  STATIC_ASSERT(( !etude::is_constructible<X, char*, void*>::value ));
  STATIC_ASSERT(( !etude::is_constructible<X, char const*, std::size_t, void>::value ));
  STATIC_ASSERT(( !etude::is_constructible<X, char const*, std::size_t, int>::value ));
  
}
