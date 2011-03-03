//
//  is_swappable のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/types/is_swappable.hpp"

#include <type_traits>

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

struct X
{
  X() = default;
  
  X( X const& ) = delete;
  void operator=( X const& ) = delete;
  
  // ユーザ定義の swap あり
  friend void swap( X&, X& ) {}
  
  // X& と int で swap 出来るように
  friend void swap( X&, int& ) {}
  friend void swap( int&, X& ) {}
  
  // char& の場合は対称律を満たさない
  friend void swap( X&, char& ) {}
  // friend void swap( X&, char& ) {}
};

struct Y
{
  Y() = default;
  
  Y( Y const& ) = delete;
  void operator=( Y const& ) = delete;
  
};
// ユーザ定義の swap なし
// 本来ならばこれは要らないが、今の swap は SFINAE してないようなので
inline void swap( Y&, Y& ) = delete;

int main()
{
  // int 型の変数は swappable
  STATIC_ASSERT(( etude::is_swappable<int>::value ));
  // int 型の値は swappable ではない
  STATIC_ASSERT(( !etude::is_value_swappable<int>::value ));
  // int& 型の値は swappable
  STATIC_ASSERT(( etude::is_value_swappable<int&>::value ));
  
  // int& と char&
  STATIC_ASSERT(( !etude::is_value_swappable<int&, char&>::value ));
  
  // ユーザ定義型
  STATIC_ASSERT(( etude::is_swappable<X>::value ));
  STATIC_ASSERT(( !etude::is_swappable<Y>::value ));
  // 非対称のユーザ定義型
  STATIC_ASSERT(( etude::is_value_swappable<X&, int&>::value ));
  STATIC_ASSERT(( etude::is_value_swappable<int&, X&>::value ));
  // 非対称の場合は対称律を満たさないとダメ
  STATIC_ASSERT(( !etude::is_value_swappable<X&, char&>::value ));
  STATIC_ASSERT(( !etude::is_value_swappable<char&, X&>::value ));
}
