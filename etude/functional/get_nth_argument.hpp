//
//  get_nth_argument:
//    N+1 番目の引数を返す
// 
//    与えられた引数が unpacked_tuple の場合は、その N 番目の要素を返します。
//    そうでない場合は、与えられた引数の N 番目を返します。
//    N は 0 ベースです。
//  
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_FUNTIONAL_INCLUDED_GET_NTH_ARGUMENT_HPP_
#define ETUDE_FUNTIONAL_INCLUDED_GET_NTH_ARGUMENT_HPP_

#include "../types/nth_type.hpp"
#include <utility>
#include <type_traits>

#include "unpacked_tuple.hpp"
#include "group.hpp"
#include "../types/head_type.hpp"
#include "../types/is_unpacked_tuple.hpp"
#include "../types/tuple_element.hpp"

namespace etude {

  // 実装用ファンクタ
  template<std::size_t N>
  class get_nth_argument_;
  
  template<>
  struct get_nth_argument_<0>
  {
    template<class T, class... Args>
    T&& operator()( T && x, Args&&... ) const {
      return std::forward<T>(x);
    }
  };
  
  template<std::size_t N>
  struct get_nth_argument_
  {
    template<class T, class... Args>
    typename nth_type<N-1, Args...>::type&& operator()( T&&, Args&&... args ) const {
      return get_nth_argument_<N-1>()( std::forward<Args>(args)... );
    }
  };
  
  // 本体
  template<std::size_t N, class... Args>
  inline typename std::enable_if<
    !etude::is_unpacked_tuple<Args...>::value,
    typename nth_type<N, Args...>::type &&
  >::type
    get_nth_argument( Args&&... args )
  {
    return get_nth_argument_<N>()( std::forward<Args>(args)... );
  }
  
  // unpack
  template<std::size_t N, class Tuple, std::size_t... Indices>
  inline typename etude::nth_type< N,
    typename etude::tuple_element<Indices, Tuple>::type...
  >::type
    get_nth_argument( unpacked_tuple<Tuple, Indices...> t )
  {
    return get_nth_argument_<N>()( etude::move<Indices>(t)...  );
  }
  // 拡張 unpack
  template<std::size_t N, class T, class... Args>
  inline typename std::enable_if<
    etude::is_unpacked_tuple<Args...>::value,
    decltype(
      etude::get_nth_argument<N>(
        etude::group( std::declval<T>(), std::declval<Args...>() )
      )
    )
  >::type
    get_nth_argument( T && t, Args&&... args )
  {
    return etude::get_nth_argument<N>(
      etude::group( std::forward<T>(t), std::forward<Args>(args)... )
    );
  }

} // namespace etude

#endif  // #ifndef ETUDE_FUNTIONAL_INCLUDED_GET_NTH_ARGUMENT_HPP_
