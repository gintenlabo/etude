//
//  etude::is_convertible_without_temporary のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/types/is_convertible_without_temporary.hpp"

#include <type_traits>
#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

template<class From, class To, bool Expected>
void check()
{
  bool const Result = etude::is_convertible_without_temporary<From, To>::value;
  
  STATIC_ASSERT(( Result == Expected ));
  
  // std::is_convertible<From, To>::value が false ならば、
  // 必ず Result も false になる
  STATIC_ASSERT ((
    ( std::is_convertible<From, To>::value == false ) ?
    ( Result == false ) : true
  ));

}

template<class From, class To, bool Expected>
void check_references()
{
  bool const Result = etude::is_convertible_without_temporary<From&, To&>::value;
  STATIC_ASSERT(( Result == Expected ));
  
  // from lvalues
  check<From&, To&, Result>();
  check<From&, To const&, Result>();
  check<From&, To volatile&, Result>();
  check<From&, To const volatile&, Result>();
  check<From&, To&&, false>();
  check<From&, To const&&, false>();
  check<From&, To volatile&&, false>();
  check<From&, To const volatile&&, false>();
  
  check<From const&, To&, false>();
  check<From const&, To const&, Result>();
  check<From const&, To volatile&, false>();
  check<From const&, To const volatile&, Result>();
  check<From const&, To&&, false>();
  check<From const&, To const&&, false>();
  check<From const&, To volatile&&, false>();
  check<From const&, To const volatile&&, false>();
  
  check<From volatile&, To&, false>();
  check<From volatile&, To const&, false>();
  check<From volatile&, To volatile&, Result>();
  check<From volatile&, To const volatile&, Result>();
  check<From volatile&, To&&, false>();
  check<From volatile&, To const&&, false>();
  check<From volatile&, To volatile&&, false>();
  check<From volatile&, To const volatile&&, false>();
  
  check<From const volatile&, To&, false>();
  check<From const volatile&, To const&, false>();
  check<From const volatile&, To volatile&, false>();
  check<From const volatile&, To const volatile&, Result>();
  check<From const volatile&, To&&, false>();
  check<From const volatile&, To const&&, false>();
  check<From const volatile&, To volatile&&, false>();
  check<From const volatile&, To const volatile&&, false>();
  
  // from rvalues
  check<From&&, To&, false>();
  check<From&&, To const&, Result>(); // ここは特例
  check<From&&, To volatile&, false>();
  check<From&&, To const volatile&, false>();
  check<From&&, To&&, Result>();
  check<From&&, To const&&, Result>();
  check<From&&, To volatile&&, Result>();
  check<From&&, To const volatile&&, Result>();
  
  check<From const&&, To&, false>();
  check<From const&&, To const&, Result>(); // ここは特例
  check<From const&&, To volatile&, false>();
  check<From const&&, To const volatile&, false>();
  check<From const&&, To&&, false>();
  check<From const&&, To const&&, Result>();
  check<From const&&, To volatile&&, false>();
  check<From const&&, To const volatile&&, Result>();
  
  check<From volatile&&, To&, false>();
  check<From volatile&&, To const&, false>();
  check<From volatile&&, To volatile&, false>();
  check<From volatile&&, To const volatile&, false>();
  check<From volatile&&, To&&, false>();
  check<From volatile&&, To const&&, false>();
  check<From volatile&&, To volatile&&, Result>();
  check<From volatile&&, To const volatile&&, Result>();
  
  check<From const volatile&&, To&, false>();
  check<From const volatile&&, To const&, false>();
  check<From const volatile&&, To volatile&, false>();
  check<From const volatile&&, To const volatile&, false>();
  check<From const volatile&&, To&&, false>();
  check<From const volatile&&, To const&&, false>();
  check<From const volatile&&, To volatile&&, false>();
  check<From const volatile&&, To const volatile&&, Result>();
  
}

// ユーザ定義の型変換
struct X
{
  int i;
  X( int i_ ) : i(i_) {}
  
  operator void *     ()       { return &i; }
  operator void const*() const { return &i; }
  
  operator int &     ()       { return i; }
  operator int const&() const { return i; }
  
};

struct Base {};
struct Derived : Base {};

#include <functional> // for std::reference_wrapper

int main()
{
  // 参照以外
  check<void, void, true>();
  check<void, int, false>();
  
  check<int, int, true>();
  check<char, int, true>();
  check<int const&, int, true>();
  check<void*, int, false>();
  
  // ユーザ定義の型変換
  check<int, X, true>();
  check<void*, X, false>();
  check<X, void*, true>();
  check<X, int*, false>();
  check<X, int&, true>();
  check<X const, int&, false>();
  check<X const, int const&, true>();
  
  // 参照
  check_references<int, int, true>();
  check_references<int, char, false>();
  check_references<int, X, false>();
  check_references<Derived, Base, true>();
  check_references<Base, Derived, false>();
  
  // 参照と値の奇妙な（実際にはそうでもない）ズレ
  check<Derived, Base, true>();
  check<Derived, Base&, false>();
  check<Derived, Base&&, true>(); // ここは微妙だけど…。
  check<Derived&, Base, true>(); // これはできるが
  check<Derived&, Base&&, false>(); // これは無理。
  
  // ユーザ定義の参照型変換
  // まず void* は参照じゃないので無理
  check<X, void*&, false>();
  check<X, void* const&, false>();
  check<X, void* &&, false>();
  
  // 本題、 int&
  check<X, int&, true>();
  check<X, int const&, true>();
  check<X, int volatile&, true>();
  check<X, int const volatile&, true>();
  check<X, int&&, false>();
  check<X, int const&&, false>();
  check<X, int volatile&&, false>();
  check<X, int const volatile&&, false>();
  
  check<X const, int&, false>();
  check<X const, int const&, true>();
  check<X const, int volatile&, false>();
  check<X const, int const volatile&, true>();
  check<X const, int&&, false>();
  check<X const, int const&&, false>();
  check<X const, int volatile&&, false>();
  check<X const, int const volatile&&, false>();
  
  check<X&, int&, true>();
  check<X&, int const&, true>();
  check<X&, int volatile&, true>();
  check<X&, int const volatile&, true>();
  check<X&, int&&, false>();
  check<X&, int const&&, false>();
  check<X&, int volatile&&, false>();
  check<X&, int const volatile&&, false>();
  
  check<X const&, int&, false>();
  check<X const&, int const&, true>();
  check<X const&, int volatile&, false>();
  check<X const&, int const volatile&, true>();
  check<X const&, int&&, false>();
  check<X const&, int const&&, false>();
  check<X const&, int volatile&&, false>();
  check<X const&, int const volatile&&, false>();
  
  check<X&&, int&, true>();
  check<X&&, int const&, true>();
  check<X&&, int volatile&, true>();
  check<X&&, int const volatile&, true>();
  check<X&&, int&&, false>();
  check<X&&, int const&&, false>();
  check<X&&, int volatile&&, false>();
  check<X&&, int const volatile&&, false>();
  
  check<X const&&, int&, false>();
  check<X const&&, int const&, true>();
  check<X const&&, int volatile&, false>();
  check<X const&&, int const volatile&, true>();
  check<X const&&, int&&, false>();
  check<X const&&, int const&&, false>();
  check<X const&&, int volatile&&, false>();
  check<X const&&, int const volatile&&, false>();


  // std::reference_wrapper のケース
  check<std::reference_wrapper<int>, int&, true>();
  check<std::reference_wrapper<int>, int const&, true>();
  check<std::reference_wrapper<int>, int volatile&, true>();
  check<std::reference_wrapper<int>, int const volatile&, true>();
  check<std::reference_wrapper<int>, int&&, false>();
  check<std::reference_wrapper<int>, int const&&, false>();
  check<std::reference_wrapper<int>, int volatile&&, false>();
  check<std::reference_wrapper<int>, int const volatile&&, false>();
  
  check<std::reference_wrapper<int const>, int&, false>();
  check<std::reference_wrapper<int const>, int const&, true>();
  check<std::reference_wrapper<int const>, int volatile&, false>();
  check<std::reference_wrapper<int const>, int const volatile&, true>();
  check<std::reference_wrapper<int const>, int&&, false>();
  check<std::reference_wrapper<int const>, int const&&, false>();
  check<std::reference_wrapper<int const>, int volatile&&, false>();
  check<std::reference_wrapper<int const>, int const volatile&&, false>();
  
  // const や参照の有無もちょっとだけチェック
  check<std::reference_wrapper<int> const,   int const&, true>();
  check<std::reference_wrapper<int> &,       int const&, true>();
  check<std::reference_wrapper<int> const&,  int const&, true>();
  check<std::reference_wrapper<int> &&,      int const&, true>();
  check<std::reference_wrapper<int> const&&, int const&, true>();
  
  check<std::reference_wrapper<int const> const,   int const&, true>();
  check<std::reference_wrapper<int const> &,       int const&, true>();
  check<std::reference_wrapper<int const> const&,  int const&, true>();
  check<std::reference_wrapper<int const> &&,      int const&, true>();
  check<std::reference_wrapper<int const> const&&, int const&, true>();
  
  check<std::reference_wrapper<int> const,   int &&, false>();
  check<std::reference_wrapper<int> &,       int &&, false>();
  check<std::reference_wrapper<int> const&,  int &&, false>();
  check<std::reference_wrapper<int> &&,      int &&, false>();
  check<std::reference_wrapper<int> const&&, int &&, false>();
  
  check<std::reference_wrapper<int const> const,   int const&&, false>();
  check<std::reference_wrapper<int const> &,       int const&&, false>();
  check<std::reference_wrapper<int const> const&,  int const&&, false>();
  check<std::reference_wrapper<int const> &&,      int const&&, false>();
  check<std::reference_wrapper<int const> const&&, int const&&, false>();
}
