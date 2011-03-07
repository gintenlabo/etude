//
//  last:
//    最後の引数を返す
// 
//    与えられた引数が unpacked_tuple の場合は、その最後の要素を返します。
//    そうでない場合は、与えられた最後の引数を返します。
//  
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_UTILITY_INCLUDED_LAST_HPP_
#define ETUDE_UTILITY_INCLUDED_LAST_HPP_

#include "get_nth.hpp"
#include <utility>
#include <type_traits>

#include "unpacked_tuple.hpp"
#include "../types/is_unpacked_tuple.hpp"
#include "../types/tuple_element.hpp"

namespace etude {

  struct last_t
  {
    // 通常版
    template< class... Args,
      std::size_t I = sizeof...(Args) - 1,
      class = typename std::enable_if<!is_unpacked_tuple<Args...>::value>::type
    >
    typename nth_type<I, Args...>::type&& operator()( Args&&... args ) const {
      return etude::get_nth_<I>()( std::forward<Args>(args)... );
    }
    
    // unpack
    template< class Tuple, std::size_t... Indices,
      std::size_t I = sizeof...(Indices) - 1,
      class Result = typename nth_type< I,
        typename tuple_element<Indices, Tuple>::type...
      >::type
    >
    Result operator()( unpacked_tuple<Tuple, Indices...> t ) const {
      return etude::get_nth_<I>()( etude::move<Indices>(t)... );
    }
    
  };
  
  namespace {
    last_t const last = {};
  }

} // namespace etude

#endif  // #ifndef ETUDE_UTILITY_INCLUDED_LAST_HPP_
