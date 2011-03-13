//
//  is_assignable_or_convertible のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/types/is_assignable_or_convertible.hpp"

#include <type_traits>

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

struct X {};

struct Y
{
  Y& operator=( X && ) {
    return *this;
  }
};

int main()
{
  // int <- int 。 おｋ
  STATIC_ASSERT(( etude::is_assignable_or_convertible<int, int>::value ));
  // int <- char
  STATIC_ASSERT(( etude::is_assignable_or_convertible<int, char>::value ));
  
  // int const <- int も OK
  STATIC_ASSERT(( etude::is_assignable_or_convertible<int const, int>::value ));
  
  // X <- Y は NG
  STATIC_ASSERT(( !etude::is_assignable_or_convertible<X, Y>::value ));
  
  // Y <- X は OK
  STATIC_ASSERT(( etude::is_assignable_or_convertible<Y, X>::value ));
  // Y <- X const& は NG
  STATIC_ASSERT(( !etude::is_assignable_or_convertible<Y, X const&>::value ));
  
  
  // 参照の場合は is_convertible と同じ
  STATIC_ASSERT(( !etude::is_assignable_or_convertible<int&, int>::value ));
  STATIC_ASSERT((  etude::is_assignable_or_convertible<int&, int&>::value ));
  STATIC_ASSERT(( !etude::is_assignable_or_convertible<int&, int&&>::value ));
  STATIC_ASSERT((  etude::is_assignable_or_convertible<int&&, int>::value ));
  STATIC_ASSERT(( !etude::is_assignable_or_convertible<int&&, int&>::value ));
  STATIC_ASSERT((  etude::is_assignable_or_convertible<int&&, int&&>::value ));
  
  // 参照でもオブジェクトでもない場合
  STATIC_ASSERT((  etude::is_assignable_or_convertible<void, void>::value ));
  STATIC_ASSERT(( !etude::is_assignable_or_convertible<void, int>::value ));
  STATIC_ASSERT(( !etude::is_assignable_or_convertible<void, int&>::value ));
  STATIC_ASSERT(( !etude::is_assignable_or_convertible<void(), void()>::value ));
  STATIC_ASSERT(( !etude::is_assignable_or_convertible<void(), void>::value ));
  STATIC_ASSERT(( !etude::is_assignable_or_convertible<void(), int>::value ));
  
}
