//
//  tuple_cat:
//    渡された Tuple を結合した std::tuple を返す
// 
//    渡されたタプルのようなものを結合して std::tuple に変換します。
//  
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_UTILITY_INCLUDED_TUPLE_CAT_HPP_
#define ETUDE_UTILITY_INCLUDED_TUPLE_CAT_HPP_

#include "../unpacked_tuple.hpp"
#include "../unpack.hpp"
#include "../types/tuple_element.hpp"
#include "tuple_get.hpp"

#include <utility>
#include <type_traits>

namespace etude {

  // 実装、兼インデックス指定版
  template<
    class Tuple1, std::size_t... Indices1,
    class Tuple2, std::size_t... Indices2,
    class Result = std::tuple<
      typename etude::tuple_element< Indices1,
        typename std::decay<Tuple1>::type
      >::type...,
      typename etude::tuple_element< Indices2,
        typename std::decay<Tuple2>::type
      >::type...
    >
  >
  inline Result tuple_cat_(
    unpacked_tuple<Tuple1, Indices1...> && t1, unpacked_tuple<Tuple2, Indices2...> && t2
  ){
    (void)t1, (void)t2;  // 警告避け
    return Result( etude::move<Indices1>(t1)..., etude::move<Indices2>(t2)... );
  }

  // 本体
  template< class Tuple1, class Tuple2,
    class Result = decltype(
      etude::tuple_cat_(
        etude::unpack( std::declval<Tuple1>() ),
        etude::unpack( std::declval<Tuple2>() )
      )
    )
  >
  inline Result tuple_cat( Tuple1 && t1, Tuple2 && t2 ) {
    return etude::tuple_cat_(
      etude::unpack( std::forward<Tuple1>(t1) ),
      etude::unpack( std::forward<Tuple2>(t2) )
    );
  }

} // namespace etude

#endif  // #ifndef ETUDE_UTILITY_INCLUDED_TUPLE_CAT_HPP_
