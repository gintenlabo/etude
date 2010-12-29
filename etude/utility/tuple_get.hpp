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

namespace etude {

  template< std::size_t I, class Tuple,
    class Result = typename etude::tuple_element<I, Tuple>::type &&
  >
  inline Result tuple_get( Tuple && t ) {
    using namespace std;
    return static_cast<Result>( get<I>(t) );
  }

} // namespace etude

#endif  // #ifndef ETUDE_UTILITY_INCLUDED_TUPLE_GET_HPP_
