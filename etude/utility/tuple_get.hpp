//
//  tuple_get:
//    タプルからの値の get
// 
//    ・現行の gcc が && 版の std::get に対応していないので、それに対応
//    ・直接 std::get って呼ぶと std::get が dependent name にならないので
//      std::array 等を後から include した場合に std::get が見つからない問題にも対処
//  
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_UTILITY_INCLUDED_TUPLE_GET_HPP_
#define ETUDE_UTILITY_INCLUDED_TUPLE_GET_HPP_

#include "../types/tuple_element.hpp"
#include <utility>
#include <type_traits>

namespace etude {

  // t の型に合わせて要素を get
  template< std::size_t I, class Tuple,
    class Elem = typename etude::tuple_element<I, Tuple>::type
  >
  inline Elem&& tuple_get( Tuple && t ) {
    using namespace std;
    return std::forward<Elem>( get<I>(t) );
  }
  
  // get してから要素を move する
  template< std::size_t I, class Tuple,
    class Elem = typename etude::tuple_element< I,
      typename std::remove_reference<Tuple>::type
    >::type
  >
  inline Elem&& tuple_move( Tuple && t ) {
    return std::forward<Elem>( tuple_get<I>(t) );
  }
  
  // get してから要素を Tuple の型に合わせて forward する
  template< class Tuple, std::size_t I, class Tuple_,
    class Elem = typename etude::tuple_element<I, Tuple>::type
  >
  inline Elem&& tuple_forward( Tuple_ && t ) {
    return std::forward<Elem>( tuple_get<I>(t) );
  }

} // namespace etude

#endif  // #ifndef ETUDE_UTILITY_INCLUDED_TUPLE_GET_HPP_
