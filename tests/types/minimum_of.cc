//
//  minimum_of のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/types/minimum_of.hpp"
#include <type_traits>

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

template<class T, T Min, class Tested>
void check( Tested ) {
  STATIC_ASSERT(( std::is_base_of<std::integral_constant<T, Min>, Tested>::value ));
}

int main()
{
  check<int, 1>( etude::minimum_of<int, 1>() );
  
  check<int, 1>( etude::minimum_of<int, 1, 1>() );
  check<int, 1>( etude::minimum_of<int, 1, 2>() );
  check<int, 1>( etude::minimum_of<int, 2, 1>() );
  
  check<int, 0>( etude::minimum_of<int, 0, 0, 0>() );
  check<int, 0>( etude::minimum_of<int, 0, 1, 1>() );
  check<int, 1>( etude::minimum_of<int, 1, 2, 3>() );
  check<int, 4>( etude::minimum_of<int, 5, 6, 4>() );
  
  check<std::size_t, sizeof(char)>(
    etude::minimum_of<std::size_t,
      sizeof(char), sizeof(int), sizeof(long), sizeof(short) >()
  );
  check<int, 0>(
    etude::minimum_of<int, 1, 9, 8, 5, 0, 6, 1, 7>()
  );
  
  // bool を扱った場合、多入力の論理積になる
  check<bool, false>( etude::minimum_of<bool, false, false, false>() );
  
  check<bool, false>( etude::minimum_of<bool, true,  false, false>() );
  check<bool, false>( etude::minimum_of<bool, false, true,  false>() );
  check<bool, false>( etude::minimum_of<bool, false, false, true >() );
  
  check<bool, false>( etude::minimum_of<bool, true,  true,  false>() );
  check<bool, false>( etude::minimum_of<bool, false, true,  true >() );
  check<bool, false>( etude::minimum_of<bool, true,  false, true >() );
  
  check<bool, true >( etude::minimum_of<bool, true,  true,  true >() );
}
