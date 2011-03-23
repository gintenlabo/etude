//
//  tuple_init:
//    タプルの最後の値を除いたタプルを取得する
// 
//    std::tuple<Init..., Last> から std::tuple<Init...> を得ます。
//    実際には std::tuple 以外も対応できます。
//    空 tuple の場合には SFINAE によって候補から外されます。
//  
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_UTILITY_INCLUDED_TUPLE_INIT_HPP_
#define ETUDE_UTILITY_INCLUDED_TUPLE_INIT_HPP_

#include "../types/tuple_indices.hpp"
#include "../types/init_types.hpp"
#include "../types/make_indices_from_types.hpp"
#include "to_tuple.hpp"

#include <utility>
#include <type_traits>

namespace etude {

  // 実装
  template< class Tuple, std::size_t... Indices,
    class InitTypes = etude::init_types< etude::size_constant<Indices>... >,
    class InitIndices = typename etude::make_indices_from_types<InitTypes>::type,
    class Result = decltype(
      etude::to_tuple( std::declval<Tuple>(), InitIndices() )
    )
  >
  inline Result tuple_init_( Tuple && t, etude::indices<Indices...> ) {
    return etude::to_tuple( std::forward<Tuple>(t), InitIndices() );
  }

  // t の先頭要素以外を詰めた tuple を得る
  template< class Tuple,
    class Result = decltype(
      etude::tuple_init_( std::declval<Tuple>(), etude::tuple_indices<Tuple>() )
    )
  >
  inline Result tuple_init( Tuple && t ) {
    return etude::tuple_init_( std::forward<Tuple>(t), etude::tuple_indices<Tuple>() );
  }

} // namespace etude

#endif  // #ifndef ETUDE_UTILITY_INCLUDED_TUPLE_INIT_HPP_
