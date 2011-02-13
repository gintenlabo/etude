//
//  bool_constant のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/types/bool_constant.hpp"

#include <type_traits>

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

// etude::bool_constant をチェックする
template<bool b>
void check()
{
  typedef etude::bool_constant<b> bool_constant;
  
  STATIC_ASSERT(( 
    std::is_base_of<
      std::integral_constant<bool, b>, bool_constant
    >::value
  ));
  STATIC_ASSERT((
    std::is_same<
      std::integral_constant<bool, b>, typename bool_constant::type
    >::value
  ));
}

int main()
{
  check<true>();
  check<false>();
  check<0>();
  check<1>();
}
