//
//  is_tuple のテストです。
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/types/is_tuple.hpp"

#include <utility>
#include <array>

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

int main()
{
  STATIC_ASSERT(( etude::is_tuple<int>::value == false ));
  STATIC_ASSERT(( etude::is_tuple<int&>::value == false ));
  
  STATIC_ASSERT(( etude::is_tuple<std::tuple<>>::value == true ));
  STATIC_ASSERT(( etude::is_tuple<std::tuple<int>>::value == true ));
  STATIC_ASSERT(( etude::is_tuple<std::tuple<int, double, char*>>::value == true ));
  STATIC_ASSERT(( etude::is_tuple<std::pair<double, char*>>::value == true ));
  STATIC_ASSERT(( etude::is_tuple<std::array<int, 0>>::value == true ));
  STATIC_ASSERT(( etude::is_tuple<std::array<int, 5>>::value == true ));
  
  STATIC_ASSERT(( etude::is_tuple<std::tuple<>&>::value == false ));
}
