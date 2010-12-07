//
//  indices のテストです。
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../etude/indices.hpp"

#include <type_traits>

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

// etude::indices をチェックする
template<std::size_t... Indices>
void check_traits( etude::indices<Indices...> )
{
  typedef etude::indices<Indices...> indices_type;
  
  // 基本的な性質
  STATIC_ASSERT(( std::is_empty<indices_type>::value ));
  STATIC_ASSERT(( std::is_trivial<indices_type>::value ));
  
  // size
  STATIC_ASSERT(( indices_type::size == sizeof...(Indices) ));
}

int main()
{
  // 基本的な性質を調べる
  check_traits( etude::indices<>() );
  check_traits( etude::indices<0>() );
  check_traits( etude::indices<0, 1, 2>() );
  // 順に並んでなくても良い
  check_traits( etude::indices<2, 0, 1, 0, 1, 2, 0, 7>() );
  // make_indices<N>() は indices<0..N-1> に暗黙変換できる
  check_traits( etude::make_indices<10>() );
  
  // make indices
  STATIC_ASSERT((
    std::is_same<
      etude::make_indices<0>::type, etude::indices<>
    >::value
  ));
  
  STATIC_ASSERT((
    std::is_same<
      etude::make_indices<1>::type, etude::indices<0>
    >::value
  ));
  
  STATIC_ASSERT((
    std::is_same<
      etude::make_indices<7>::type, etude::indices<0, 1, 2, 3, 4, 5, 6>
    >::value
  ));
}
