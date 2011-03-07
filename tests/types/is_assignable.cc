//
//  etude::is_assignable のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/types/is_assignable.hpp"

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
  STATIC_ASSERT(( etude::is_assignable<int, int>::value ));
  // int <- char
  STATIC_ASSERT(( etude::is_assignable<int, char>::value ));
  
  // int const <- int は NG
  STATIC_ASSERT(( !etude::is_assignable<int const, int>::value ));
  
  // X <- Y は NG
  STATIC_ASSERT(( !etude::is_assignable<X, Y>::value ));
  
  // Y <- X は OK
  STATIC_ASSERT(( etude::is_assignable<Y, X>::value ));
  // Y <- X const& は NG
  STATIC_ASSERT(( !etude::is_assignable<Y, X const&>::value ));
  
}
