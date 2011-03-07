//
//  get_type_or のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/types/get_type_or.hpp"

#include <type_traits>

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

// チェック関数
template<class T, class Expected>
void check()
{
  typedef typename etude::get_type_or<T>::type type;
  STATIC_ASSERT(( std::is_same<type, Expected>::value ));
}

// ユーザ定義型
struct X{ typedef int type; };
struct Y{};
struct Z{ static const int type = 0; };

int main()
{
  // 通常のメタ関数
  check< std::is_void<void>, std::true_type >();
  check< std::add_pointer<void>, void* >();
  
  // ユーザ定義型
  check< X, int >();
  check< Y, void >();
  check< Z, void >();
  
  // その他の型
  check< void, void >();
  check< int, void >();
  check< X const, int >();
  check< X *, void >();
  check< X &, void >();

  // Default 指定がある場合
  STATIC_ASSERT((
    std::is_same<
      etude::get_type_or<Y, int>::type, int
    >::value
  ));
  STATIC_ASSERT((
    std::is_same<
      etude::get_type_or<std::enable_if<false>, X&>::type, X&
    >::value
  ));
}
