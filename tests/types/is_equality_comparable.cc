//
//  is_equality_comparable のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/types/is_equality_comparable.hpp"

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

struct convertible_to_bool
{
  bool value;
  
  explicit convertible_to_bool( bool b )
    : value( b ) {}
  
  // explicitly convertible to bool
  explicit operator bool() const { return value; }
  
};

struct X
{
  int value;
  
  explicit X( int value_ )
    : value( value_ ) {}
  
  friend convertible_to_bool operator==( X const& lhs, X const& rhs ) {
    return convertible_to_bool( lhs.value == rhs.value );
  }
  
};

// 変な例
struct Y
{
  int value;
  
  explicit Y( int value_ )
    : value( value_ ) {}
  
  // mutable な参照を取る operator== しかない
  
  // lvalue
  friend bool operator==( Y& lhs, Y& rhs ) {
    return lhs.value == rhs.value;
  }
  
};

int main()
{
  // 組み込み型
  STATIC_ASSERT((  etude::is_equality_comparable<int>::value ));
  STATIC_ASSERT((  etude::is_equality_comparable<void*>::value ));
  STATIC_ASSERT(( !etude::is_equality_comparable<int, int*>::value ));
  STATIC_ASSERT(( !etude::is_equality_comparable<int*, int>::value ));
  STATIC_ASSERT((  etude::is_equality_comparable<int*, void*>::value ));
  STATIC_ASSERT((  etude::is_equality_comparable<void*, int*>::value ));
  STATIC_ASSERT(( !etude::is_equality_comparable<int*, char*>::value ));
  STATIC_ASSERT(( !etude::is_equality_comparable<char*, int*>::value ));
  
  STATIC_ASSERT((  etude::is_equality_comparable<int const*, void*>::value ));
  STATIC_ASSERT(( !etude::is_equality_comparable<char const*, int*>::value ));
  
  // 参照を渡した場合、組み込み型の場合は結果は変わらない
  STATIC_ASSERT((  etude::is_equality_comparable<int&>::value ));
  STATIC_ASSERT((  etude::is_equality_comparable<void*&>::value ));
  STATIC_ASSERT(( !etude::is_equality_comparable<int&, int*&>::value ));
  STATIC_ASSERT(( !etude::is_equality_comparable<int*&, int&>::value ));
  STATIC_ASSERT((  etude::is_equality_comparable<int*&, void*&>::value ));
  STATIC_ASSERT((  etude::is_equality_comparable<void*&, int*&>::value ));
  STATIC_ASSERT(( !etude::is_equality_comparable<int*&, char*&>::value ));
  STATIC_ASSERT(( !etude::is_equality_comparable<char*&, int*&>::value ));
  
  // 配列
  STATIC_ASSERT((  etude::is_equality_comparable<int[5]>::value ));
  STATIC_ASSERT((  etude::is_equality_comparable<int[5], int(&)[3]>::value ));
  STATIC_ASSERT((  etude::is_equality_comparable<int[5], int const[4]>::value ));
  STATIC_ASSERT(( !etude::is_equality_comparable<int[5], char[5]>::value ));
  STATIC_ASSERT(( !etude::is_equality_comparable<char[5], int[5]>::value ));
  
  // ユーザ定義型
  // operator == の戻り値が bool に変換できる場合
  STATIC_ASSERT((  etude::is_equality_comparable<X>::value ));
  // 関係ない型の場合（ int -> X の変換は explicit なので無理）
  STATIC_ASSERT(( !etude::is_equality_comparable<X, int>::value ));
  // operator == が mutable な参照を取る場合はそのままでは比較できない
  STATIC_ASSERT(( !etude::is_equality_comparable<Y>::value ));
  // その場合は明示的に参照にする
  STATIC_ASSERT((  etude::is_equality_comparable<Y&>::value ));
  // rvalue reference にも対応（ Y の場合は無理だけど）
  STATIC_ASSERT(( !etude::is_equality_comparable<Y&&>::value ));
  
  // volatile 対応
  STATIC_ASSERT((  etude::is_equality_comparable<int volatile>::value ));
  STATIC_ASSERT(( !etude::is_equality_comparable<  X volatile>::value ));
  
}
