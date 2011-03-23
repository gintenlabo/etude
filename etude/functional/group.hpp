//
//  group:
//    pack して unpack する
// 
//    与えられた引数を unpacked_tuple にまとめます。
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
#include "pack.hpp"
#include "../types/is_unpacked_tuple.hpp"

namespace etude {

  struct group_t
  {
    // 通常版
    template< class... Args,
      class = typename std::enable_if<
        !etude::is_unpacked_tuple<Args...>::value
      >::type,
      class Result = typename etude::unpacked<std::tuple<Args&&...>>::type
    >
    Result operator()( Args&&... args ) const {
      return Result( std::forward<Args>(args)... );
    }
    
    // 与えられた引数リストが unpack で終わる場合
    template< class T, class... Args,
      class = typename std::enable_if<
        etude::is_unpacked_tuple<T, Args...>::value
      >::type,
      class Tuple = decltype(
        etude::pack( std::declval<T>(), std::declval<Args>()... )
      ),
      class Result = typename etude::unpacked<Tuple>::type
    >
    Result operator()( T && t, Args&&... args ) const {
      return Result(
        etude::pack( std::forward<T>(t), std::forward<Args>(args)... )
      );
    }
    
  };
  
  namespace {
    group_t const group = {};
  }
  
} // namespace etude

#endif  // #ifndef ETUDE_FUNCTIONAL_INCLUDED_GROUP_HPP_
