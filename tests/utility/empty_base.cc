//
//  empty_base のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/utility/empty_base.hpp"

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )
#include <type_traits>

template<class T>
inline void check()
{
  typedef etude::empty_base<T> tested;
  
  STATIC_ASSERT(( std::is_empty<tested>::value ));
  STATIC_ASSERT(( std::is_trivial<tested>::value ));
}

int main()
{
  check<void>();
  check<int>();
  
  class X {};
  check<X>();
  check<X*>();
  check<X&>();
  check<X&&>();
  
  // それぞれ独立したクラス
  STATIC_ASSERT(( !std::is_same< etude::empty_base<>, etude::empty_base<int> >::value ));
  
}
