//
//  all_of_c のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/types/all_of_c.hpp"
#include <type_traits>

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

template<bool Cond, class Tested>
void check( Tested ) {
  STATIC_ASSERT(( std::is_base_of<std::integral_constant<bool, Cond>, Tested>::value ));
}

int main()
{
  // 要素なし
  check<true>( etude::all_of_c<>() );
  
  // 一要素
  check<false>( etude::all_of_c<false>() );
  check<true >( etude::all_of_c<true >() );
  
  // 二要素
  check<false>( etude::all_of_c<false, false>() );
  check<false>( etude::all_of_c<true,  false>() );
  check<false>( etude::all_of_c<false, true >() );
  check<true >( etude::all_of_c<true,  true >() );
  
  // 三要素
  check<false>( etude::all_of_c<false, false, false>() );
  
  check<false>( etude::all_of_c<true,  false, false>() );
  check<false>( etude::all_of_c<false, true,  false>() );
  check<false>( etude::all_of_c<false, false, true >() );
  
  check<false>( etude::all_of_c<true,  true,  false>() );
  check<false>( etude::all_of_c<false, true,  true >() );
  check<false>( etude::all_of_c<true,  false, true >() );
  
  check<true >( etude::all_of_c<true,  true,  true >() );
  
  // たくさん
  check<false>( etude::all_of_c<true, true, true, true, false, true>() );
  check<true>( etude::all_of_c<true, true, true, true, true, true, true>() );
}
