//
//  maximum_of のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/types/maximum_of.hpp"
#include <type_traits>

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

template<class T, T Max, class Tested>
void check( Tested ) {
  STATIC_ASSERT(( std::is_base_of<std::integral_constant<T, Max>, Tested>::value ));
}

int main()
{
  check<int, 1>( etude::maximum_of<int, 1>() );
  
  check<int, 1>( etude::maximum_of<int, 1, 1>() );
  check<int, 2>( etude::maximum_of<int, 1, 2>() );
  check<int, 2>( etude::maximum_of<int, 2, 1>() );
  
  check<int, 0>( etude::maximum_of<int, 0, 0, 0>() );
  check<int, 1>( etude::maximum_of<int, 0, 1, 1>() );
  check<int, 3>( etude::maximum_of<int, 1, 2, 3>() );
  check<int, 6>( etude::maximum_of<int, 5, 6, 4>() );
  
  check<std::size_t, sizeof(long)>(
    etude::maximum_of<std::size_t,
      sizeof(char), sizeof(int), sizeof(long), sizeof(short) >()
  );
  check<int, 9>(
    etude::maximum_of<int, 1, 9, 8, 5, 0, 6, 1, 7>()
  );
  
  // bool を扱った場合、多入力の論理和になる
  check<bool, false>( etude::maximum_of<bool, false, false, false>() );
  
  check<bool, true >( etude::maximum_of<bool, true,  false, false>() );
  check<bool, true >( etude::maximum_of<bool, false, true,  false>() );
  check<bool, true >( etude::maximum_of<bool, false, false, true >() );
  
  check<bool, true >( etude::maximum_of<bool, true,  true,  false>() );
  check<bool, true >( etude::maximum_of<bool, false, true,  true >() );
  check<bool, true >( etude::maximum_of<bool, true,  false, true >() );
  
  check<bool, true >( etude::maximum_of<bool, true,  true,  true >() );
}
