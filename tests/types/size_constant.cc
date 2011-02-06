//
//  size_constant のテストです。
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/types/size_constant.hpp"

#include <type_traits>

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

// etude::size_constant をチェックする
template<std::size_t N>
void check()
{
  typedef etude::size_constant<N> size_constant;
  
  STATIC_ASSERT(( 
    std::is_base_of<
      std::integral_constant<std::size_t, N>, size_constant
    >::value
  ));
  STATIC_ASSERT((
    std::is_same<
      std::integral_constant<std::size_t, N>, typename size_constant::type
    >::value
  ));
}

int main()
{
  check<0>();
  check<1>();
  check<2>();
  check<23>();
  check<42>();
  check<-1>();
}
