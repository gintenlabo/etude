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
#include "../types/tuple_element.hpp"
#include "../types/last_type.hpp"
#include "../utility/forward_as_tuple.hpp"
#include "../utility/tuple_init.hpp"
#include "../utility/tuple_last.hpp"
#include "../utility/tuple_cat.hpp"

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
    template< class... Args,
      class Tuple1 = std::tuple<Args&&...>,
      class Init   = decltype( etude::tuple_init( std::declval<Tuple1>() ) ),
      class Last   = decltype( etude::tuple_last( std::declval<Tuple1>() ) ),
      class Last_  = decltype( etude::pack( std::declval<Last>() ) ),
      class Tuple2 = decltype(
        etude::tuple_cat( std::declval<Init>(), std::declval<Last_>() )
      ),
      class Result = typename etude::unpacked<Tuple2>::type,
      class = typename std::enable_if<
        etude::is_unpacked_tuple<Args...>::value
      >::type
    >
    Result operator()( Args&&... args ) const {
      auto t = etude::forward_as_tuple( std::forward<Args>(args)... );
      auto init = etude::tuple_init( std::move(t) );
      auto last = etude::pack( etude::tuple_last( std::move(t) ) );
      return Result(
        etude::tuple_cat( std::move(init), std::move(last) )
      );
    }
    
  };
  
  namespace {
    group_t const group = {};
  }
  
} // namespace etude

#endif  // #ifndef ETUDE_FUNCTIONAL_INCLUDED_GROUP_HPP_
