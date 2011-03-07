//
//  operator_arrow のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/utility/operator_arrow.hpp"

#include <type_traits>

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

// for std::unique_ptr
#include <memory>

// 俺々クラス
struct X
{
  int i;
  
  int      * operator->()       { return &i; }
  int const* operator->() const { return &i; }
  
};

int main()
{
  // ダメ
  // int* p0 = 0;
  // etude::operator_arrow( p0 );
  
  X* p1 = 0;
  STATIC_ASSERT((
    std::is_same<
      decltype( etude::operator_arrow( p1 ) ), X*
    >::value
  ));
  
  std::unique_ptr<X> p2;
  STATIC_ASSERT((
    std::is_same<
      decltype( etude::operator_arrow( p2 ) ), X*
    >::value
  ));
  
  STATIC_ASSERT((
    std::is_same<
      decltype( etude::operator_arrow( X() ) ), int*
    >::value
  ));
  X const x = {1};
  STATIC_ASSERT((
    std::is_same<
      decltype( etude::operator_arrow( x ) ), int const*
    >::value
  ));
}
