//
//  make_struct のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/types/make_struct.hpp"

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )
#include <type_traits>

template<class T>
void check()
{
  typedef typename etude::make_struct<T>::type struct_t;
  
  STATIC_ASSERT(( std::is_class<struct_t>::value ));
}

int main()
{
  check<int>();
  check<int const>();
  check<int*>();
  check<int&>();
}
