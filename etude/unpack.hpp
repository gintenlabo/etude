//
//  unpack:
//    渡されたタプルから etude::unpacke_tuple を作る
// 
//    簡単に std::unique_ptr を作るためのヘルパ関数です。
//    to_unique という名前でしたが、より短く分かりやすく。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_INCLUDED_UNPACK_HPP_
#define ETUDE_INCLUDED_UNPACK_HPP_

#include "utility/unpacked_tuple.hpp"
#include "types/indices.hpp"
#include "types/tuple_indices.hpp"

#include <utility>

namespace etude {
  
  // unpack の適用結果を返すメタ関数
  template<class T, class Indices>
  struct unpacked_ {};
  
  template<class Tuple, std::size_t... Indices>
  struct unpacked_< Tuple, etude::indices<Indices...> >
  {
    typedef unpacked_tuple<Tuple, Indices...> type;
  };
  
  template<class T>
  struct unpacked
    : unpacked_< T, typename etude::tuple_indices<T>::type >
  {
  };
  
  // 本体
  template<class Tuple>
  inline typename unpacked<Tuple&&>::type unpack( Tuple && t )
  {
    return std::forward<Tuple>(t);
  }

} // namespace etude

#endif  // #ifndef ETUDE_INCLUDED_UNPACK_HPP_
