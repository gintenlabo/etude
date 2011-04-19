//
//  etude::is_unpack_constructible のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/types/is_unpack_constructible.hpp"

#include <type_traits>
#include <utility>
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
  STATIC_ASSERT(( !etude::is_unpack_constructible<void, void>::value ));
  STATIC_ASSERT(( !etude::is_unpack_constructible<void, std::tuple<>>::value ));
  STATIC_ASSERT(( !etude::is_unpack_constructible<void const, std::tuple<>&>::value ));
  STATIC_ASSERT(( !etude::is_unpack_constructible<void, std::tuple<int>>::value ));
  STATIC_ASSERT(( !etude::is_unpack_constructible<void, std::tuple<void>>::value ));
  STATIC_ASSERT(( !etude::is_unpack_constructible<void, std::pair<int, char>>::value ));
  STATIC_ASSERT(( !etude::is_unpack_constructible<void(), std::tuple<>>::value ));
  STATIC_ASSERT(( !etude::is_unpack_constructible<void(), std::tuple<int>>::value ));
  STATIC_ASSERT(( !etude::is_unpack_constructible<void(), std::pair<double, int*>&>::value ));
  
  // arithmetic types
  STATIC_ASSERT(( !etude::is_unpack_constructible<int, int>::value ));
  STATIC_ASSERT((  etude::is_unpack_constructible<int, std::tuple<>>::value ));
  STATIC_ASSERT((  etude::is_unpack_constructible<int const, std::tuple<>&>::value ));
  STATIC_ASSERT((  etude::is_unpack_constructible<int, std::tuple<int>>::value ));
  STATIC_ASSERT((  etude::is_unpack_constructible<int, std::tuple<char>&>::value ));
  STATIC_ASSERT(( !etude::is_unpack_constructible<int, std::tuple<void>>::value ));
  STATIC_ASSERT(( !etude::is_unpack_constructible<int, std::tuple<void*>>::value ));
  STATIC_ASSERT(( !etude::is_unpack_constructible<int, std::pair<int, int>>::value ));
  STATIC_ASSERT((  etude::is_unpack_constructible<double, std::tuple<>>::value ));
  STATIC_ASSERT((  etude::is_unpack_constructible<double, std::tuple<int>>::value ));
  STATIC_ASSERT((  etude::is_unpack_constructible<double, std::tuple<double>>::value ));
  STATIC_ASSERT(( !etude::is_unpack_constructible<double, std::tuple<void>>::value ));
  STATIC_ASSERT(( !etude::is_unpack_constructible<double, std::tuple<double*>>::value ));
  STATIC_ASSERT(( !etude::is_unpack_constructible<double, std::tuple<int, int>>::value ));
  
  // ポインタ
  STATIC_ASSERT((  etude::is_unpack_constructible<Base*, std::tuple<> const>::value ));
  STATIC_ASSERT((  etude::is_unpack_constructible<Base const*, std::tuple<>>::value ));
  STATIC_ASSERT((  etude::is_unpack_constructible<Base* const, std::tuple<>&&>::value ));
  STATIC_ASSERT((  etude::is_unpack_constructible<void*, std::tuple<Base*>>::value ));
  STATIC_ASSERT(( !etude::is_unpack_constructible<Base*, std::tuple<void*>>::value ));
  STATIC_ASSERT((  etude::is_unpack_constructible<Base*, std::tuple<Derived*>>::value ));
  STATIC_ASSERT(( !etude::is_unpack_constructible<Derived*, std::tuple<Base*>>::value ));
  STATIC_ASSERT(( !etude::is_unpack_constructible<Base**, std::tuple<Derived**>>::value ));
  STATIC_ASSERT(( !etude::is_unpack_constructible<Base*, std::pair<Derived*, int>>::value ));

  // 参照
  STATIC_ASSERT(( !etude::is_unpack_constructible<int&, std::tuple<>>::value ));
  STATIC_ASSERT(( !etude::is_unpack_constructible<int&&, std::tuple<>>::value ));
  STATIC_ASSERT((  etude::is_unpack_constructible<int&, std::tuple<int>&>::value ));
  STATIC_ASSERT(( !etude::is_unpack_constructible<int&&, std::tuple<int>&>::value ));
  STATIC_ASSERT((  etude::is_unpack_constructible<int&&, std::tuple<char&>>::value ));
  STATIC_ASSERT(( !etude::is_unpack_constructible<Base&, std::tuple<>>::value ));
  STATIC_ASSERT((  etude::is_unpack_constructible<Base&, std::tuple<Derived>&>::value ));
  STATIC_ASSERT(( !etude::is_unpack_constructible<Base&, std::tuple<Derived> const&>::value ));
  STATIC_ASSERT((  etude::is_unpack_constructible<Base const&, std::tuple<Derived&>>::value ));
  STATIC_ASSERT(( !etude::is_unpack_constructible<Base const&, std::pair<Derived, int>&>::value ));
  
  // ユーザ定義型
  STATIC_ASSERT(( !etude::is_unpack_constructible<X, std::tuple<>>::value ));
  STATIC_ASSERT(( !etude::is_unpack_constructible<X const, std::tuple<>>::value ));
  STATIC_ASSERT((  etude::is_unpack_constructible<X, std::tuple<char*>>::value ));
  STATIC_ASSERT(( !etude::is_unpack_constructible<X, std::tuple<void*>>::value ));
  STATIC_ASSERT((  etude::is_unpack_constructible<X, std::tuple<char*, int>>::value ));
  STATIC_ASSERT(( !etude::is_unpack_constructible<X, std::tuple<void*, int>>::value ));
  STATIC_ASSERT(( !etude::is_unpack_constructible<X, std::tuple<char*, void*>>::value ));
  STATIC_ASSERT(( !etude::is_unpack_constructible<X, std::tuple<char const*, std::size_t, void>>::value ));
  STATIC_ASSERT(( !etude::is_unpack_constructible<X, std::tuple<char const*, std::size_t, int>>::value ));
  
}
