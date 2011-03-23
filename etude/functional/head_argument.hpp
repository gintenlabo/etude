//
//  head_argument:
//    最初の引数を返す
// 
//    与えられた引数が unpacked_tuple の場合は、その先頭の要素を返します。
//    そうでない場合は、与えられた最初の引数を返します。
//  
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_FUNCTIONAL_INCLUDED_HEAD_ARGUMENT_HPP_
#define ETUDE_FUNCTIONAL_INCLUDED_HEAD_ARGUMENT_HPP_

#include <utility>
#include <type_traits>

#include "unpacked_tuple.hpp"
#include "../types/is_unpacked_tuple.hpp"
#include "../types/tuple_element.hpp"

namespace etude {

  struct head_argument_t
  {
    // 通常版
    template<class T, class... Args,
      class = typename std::enable_if<
        ( sizeof...(Args) > 0 ) ||
        !is_unpacked_tuple<T>::value
      >::type
    >
    T&& operator()( T && x, Args&&... ) const {
      return std::forward<T>(x);
    }
    
    // unpack
    template< class Tuple, std::size_t N, std::size_t... Indices,
      class R = typename etude::tuple_element<N, Tuple>::type
    >
    R operator()( unpacked_tuple<Tuple, N, Indices...> t ) const {
      return etude::move<N>(t);
    }
    
  };
  
  namespace {
    head_argument_t const head_argument = {};
  }

} // namespace etude

#endif  // #ifndef ETUDE_FUNCTIONAL_INCLUDED_HEAD_ARGUMENT_HPP_
