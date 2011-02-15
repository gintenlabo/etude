//
//  unpack:
//    渡されたタプルから etude::unpacke_tuple を作る
// 
//    渡されたタプルへの参照を保持する etude::unpacke_tuple を作ります。
//    これにより、 unpacke_tuple に対し特殊化されたアルゴリズムを使うことが出来ます。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_UTILITY_INCLUDED_UNPACK_HPP_
#define ETUDE_UTILITY_INCLUDED_UNPACK_HPP_

#include "unpacked_tuple.hpp"

#include "../types/indices.hpp"
#include "../types/tuple_indices.hpp"

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

#endif  // #ifndef ETUDE_UTILITY_INCLUDED_UNPACK_HPP_
