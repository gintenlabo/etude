//
//  tail_arguments:
//    最初以外の引数を unpacked_tuple にまとめて返す
// 
//    与えられた引数が unpacked_tuple の場合は、その先頭の要素を返します。
//    そうでない場合は、与えられた最初の引数を返します。
//  
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_UTILITY_INCLUDED_TAIL_ARGUMENTS_HPP_
#define ETUDE_UTILITY_INCLUDED_TAIL_ARGUMENTS_HPP_

#include <tuple>

#include <utility>
#include <type_traits>

#include "unpack.hpp"
#include "unpacked_tuple.hpp"
#include "group.hpp"
#include "../types/is_unpacked_tuple.hpp"
#include "../types/tuple_element.hpp"

namespace etude {

  struct tail_arguments_t
  {
    // 通常版
    template<class T, class... Args,
      class Result = decltype(
        etude::group( std::declval<Args>()... )
      ),
      class = typename std::enable_if<!is_unpacked_tuple<T>::value>::type
    >
    Result operator()( T&&, Args&&... args ) const {
      return etude::group( std::forward<Args>(args)... );
    }
    
    // unpack
    template< class Tuple, std::size_t N, std::size_t... Indices >
    unpacked_tuple<Tuple&&, Indices...>
      operator()( unpacked_tuple<Tuple, N, Indices...> && t ) const
    {
      return unpacked_tuple<Tuple&&, Indices...>( t.move_tuple() );
    }
    template< class Tuple, std::size_t N, std::size_t... Indices >
    unpacked_tuple<Tuple const&, Indices...>
      operator()( unpacked_tuple<Tuple, N, Indices...> const& t ) const
    {
      return unpacked_tuple<Tuple const&, Indices...>( t.get_tuple() );
    }
    
  };
  
  namespace {
    tail_arguments_t const tail_arguments = {};
  }

} // namespace etude

#endif  // #ifndef ETUDE_UTILITY_INCLUDED_TAIL_ARGUMENTS_HPP_
