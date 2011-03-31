//
//  last_argument:
//    最後の引数を返す
// 
//    与えられた引数が unpacked_tuple の場合は、その最後の要素を返します。
//    そうでない場合は、与えられた最後の引数を返します。
//  
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_FUNCTIONAL_INCLUDED_LAST_ARGUMENT_HPP_
#define ETUDE_FUNCTIONAL_INCLUDED_LAST_ARGUMENT_HPP_

#include "get_nth_argument.hpp"
#include <utility>
#include <type_traits>

#include "unpacked_tuple.hpp"
#include "group.hpp"
#include "../types/is_unpacked_tuple.hpp"
#include "../types/tuple_element.hpp"

namespace etude {

  struct last_argument_t
  {
    // 通常版
    template< class... Args,
      class = typename std::enable_if<
        !etude::is_unpacked_tuple<Args...>::value
      >::type,
      std::size_t I = sizeof...(Args) - 1
    >
    typename nth_type<I, Args...>::type&& operator()( Args&&... args ) const {
      return etude::get_nth_argument_<I>()( std::forward<Args>(args)... );
    }
    
    // unpack
    template< class Tuple, std::size_t... Indices >
    typename nth_type< sizeof...(Indices) - 1,
      typename tuple_element<Indices, Tuple>::type...
    >::type
      operator()( unpacked_tuple<Tuple, Indices...> t ) const
    {
      return etude::get_nth_argument_<sizeof...(Indices) - 1>()(
        etude::move<Indices>(t)...
      );
    }
    // 拡張 unpack
    template< class T, class... Args,
      class = typename std::enable_if<
        etude::is_unpacked_tuple<Args...>::value
      >::type,
      class Result = decltype(
        last_argument_t()( etude::group( std::declval<Args>()... ) )
      )
    >
    Result operator()( T&&, Args&&... args ) const {
      return (*this)( etude::group( std::forward<Args>(args)... ) );
    }
    
  };
  
  namespace {
    last_argument_t const last_argument = {};
  }

} // namespace etude

#endif  // #ifndef ETUDE_FUNCTIONAL_INCLUDED_LAST_ARGUMENT_HPP_
