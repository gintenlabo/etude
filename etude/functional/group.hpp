//
//  group:
//    pack して unpack する
// 
//    与えられた引数を unpacked_tuple にまとめます。
//  
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_FUNCTIONAL_INCLUDED_GROUP_HPP_
#define ETUDE_FUNCTIONAL_INCLUDED_GROUP_HPP_

#include <tuple>
#include <utility>

#include "unpacked_tuple.hpp"
#include "unpack.hpp"
#include "../types/is_unpacked_tuple.hpp"
#include "../types/tuple_element.hpp"

namespace etude {

  struct group_t
  {
    // 通常版
    template< class... Args,
      class = typename std::enable_if<!is_unpacked_tuple<Args...>::value>::type,
      class Result = typename etude::unpacked<std::tuple<Args&&...>>::type
    >
    Result operator()( Args&&... args ) const {
      return Result( std::forward<Args>(args)... );
    }
    
    // unpack 版
    template< class Tuple, std::size_t... Indices,
      class Result = typename etude::unpacked<
        std::tuple<typename etude::tuple_element<Indices, Tuple&&>::type...>
      >::type
    >
    Result operator()( unpacked_tuple<Tuple, Indices...> && t ) const {
      (void)t;  // 警告避け
      return Result( etude::move<Indices>(t)... );
    }
    template< class Tuple, std::size_t... Indices,
      class Result = typename etude::unpacked<
        std::tuple<typename etude::tuple_element<Indices, Tuple const&>::type...>
      >::type
    >
    Result operator()( unpacked_tuple<Tuple, Indices...> const& t ) const {
      (void)t;  // 警告避け
      return Result( etude::get<Indices>(t)... );
    }
    
  };
  
  namespace {
    group_t const group = {};
  }
  
} // namespace etude

#endif  // #ifndef ETUDE_FUNCTIONAL_INCLUDED_GROUP_HPP_
