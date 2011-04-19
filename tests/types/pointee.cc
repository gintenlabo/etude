//
//  pointee のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/types/pointee.hpp"

#include "../../etude/types/get_type_or.hpp"
#include <type_traits>
#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

template<class T, class Expected>
void check()
{
  STATIC_ASSERT((
    std::is_same<typename etude::pointee<T>::type, Expected>::value
  ));
  STATIC_ASSERT((
    std::is_same<typename etude::pointee<T&&>::type, Expected>::value
  ));
}
template<class T>
void check_not_defined()
{
  struct X {};
  
  STATIC_ASSERT((
    std::is_same<typename etude::get_type_or<etude::pointee<T>, X>::type, X>::value
  ));
  
}

struct Maybe
{
  explicit operator bool() /*non-const*/ { return true; }
  int operator*() /*non-const*/ { return 42; }
};

struct NonMaybeA
{
  explicit operator bool() const { return true; }
};
struct NonMaybeB
{
  int operator*() const { return 42; }
};

#include <memory> // for std::unique_ptr

int main()
{
  // 組み込み型
  check_not_defined<void>();
  check_not_defined<int>();
  check_not_defined<void*>();
  check<int*, int&>();
  check<int* const, int&>();
  check<void(*)(), void(&)()>();
  check<void(&)(), void(&)()>();
  check_not_defined<int Maybe::*>();
  
  // ユーザ定義型
  check<std::unique_ptr<int>, int&>();
  check<std::unique_ptr<int> const&, int&>();
  check<Maybe, int>();
  check<Maybe&, int>();
  check_not_defined<Maybe const>();
  check_not_defined<Maybe const&>();
  check_not_defined<NonMaybeA>();
  check_not_defined<NonMaybeA const&>();
  check_not_defined<NonMaybeB>();
  check_not_defined<NonMaybeB const&>();
}
