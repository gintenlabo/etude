//
//  pack:
//    std::forward_as_tuple の unpack 対応版
// 
//    基本的には std::forward_as_tuple と同じです。
//    すなわち、与えられた引数への rvalue reference を格納した std::tuple を作ります。
//    与えられた引数が unpacked_tuple ならば、 unpack した上で std::tuple にまとめます。
//  
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_FUNCTIONAL_INCLUDED_PACK_HPP_
#define ETUDE_FUNCTIONAL_INCLUDED_PACK_HPP_

#include <tuple>
#include <utility>

#include "unpacked_tuple.hpp"
#include "../types/is_unpacked_tuple.hpp"
#include "../types/tuple_element.hpp"

namespace etude {

  struct pack_t
  {
    // 通常版
    template< class... Args,
      class = typename std::enable_if<!is_unpacked_tuple<Args...>::value>::type
    >
    std::tuple<Args&&...> operator()( Args&&... args ) const {
      return std::tuple<Args&&...>( std::forward<Args>(args)... );
    }
    
    // unpack 版
    template< class Tuple, std::size_t... Indices,
      class Result = std::tuple<typename etude::tuple_element<Indices, Tuple&&>::type...>
    >
    Result operator()( unpacked_tuple<Tuple, Indices...> && t ) const {
      (void)t;  // 警告避け
      return Result( etude::move<Indices>(t)... );
    }
    template< class Tuple, std::size_t... Indices,
      class Result = std::tuple<
        typename etude::tuple_element<Indices, Tuple const&>::type...
      >
    >
    Result operator()( unpacked_tuple<Tuple, Indices...> const& t ) const {
      (void)t;  // 警告避け
      return Result( etude::get<Indices>(t)... );
    }
    
  };
  
  namespace {
    pack_t const pack = {};
  }
  
} // namespace etude

#endif  // #ifndef ETUDE_FUNCTIONAL_INCLUDED_PACK_HPP_
