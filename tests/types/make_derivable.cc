//
//  make_derivable のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/types/make_derivable.hpp"
#include "../../etude/types/is_derivable.hpp"

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )
#include <type_traits>

template<class T>
void check()
{
  typedef typename etude::make_derivable<T>::type derivable_t;
  
  // 出来上がった型は継承可能
  STATIC_ASSERT(( etude::is_derivable<derivable_t>::value ));
  
  // 出来上がった型から元の型へ暗黙変換できる
  STATIC_ASSERT((
    std::is_convertible<derivable_t&, T&>::value
  ));
  STATIC_ASSERT((
    std::is_convertible<derivable_t const&, T const&>::value
  ));
}

int main()
{
  check<int>();
  check<int const>();
  check<int*>();
  check<int&>();
  
  class X {};
  check<X>();
  check<X&>();
  check<X&&>();
  
  check<int [5]>();
}
