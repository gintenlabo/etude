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
#include "unpack.hpp"
#include "../types/is_unpacked_tuple.hpp"
#include "../types/tuple_element.hpp"
#include "../types/tuple_size.hpp"
#include "../types/indices.hpp"
#include "../types/last_type.hpp"
#include "../utility/tuple_cat.hpp"
#include "../utility/tuple_last.hpp"

namespace etude {

  struct pack_t
  {
    // 通常版
    template< class... Args,
      class = typename std::enable_if<
        !etude::is_unpacked_tuple<Args...>::value
      >::type
    >
    std::tuple<Args&&...> operator()( Args&&... args ) const {
      return std::tuple<Args&&...>( std::forward<Args>(args)... );
    }
    
    // 与えられた引数リストが unpack で終わる場合
    // unpack 版
    template< class Tuple, std::size_t... Indices>
    std::tuple<
      typename etude::tuple_element<Indices, Tuple&&>::type...
    >
      operator()( unpacked_tuple<Tuple, Indices...> && t ) const
    {
      typedef std::tuple<
        typename etude::tuple_element<Indices, Tuple&&>::type...
      > result_type;
      
      (void)t;  // 警告避け
      return result_type( etude::move<Indices>(t)... );
    }
    template< class Tuple, std::size_t... Indices >
    std::tuple<
      typename etude::tuple_element<Indices, Tuple const&>::type...
    >
      operator()( unpacked_tuple<Tuple, Indices...> const& t ) const
    {
      typedef std::tuple<
        typename etude::tuple_element<Indices, Tuple const&>::type...
      > result_type;
      
      (void)t;  // 警告避け
      return Result( etude::get<Indices>(t)... );
    }
    
    // 拡張 unpack 対応
    template< class T, class... Args,
      class = typename std::enable_if<
        etude::is_unpacked_tuple<Args...>::value
      >::type,
      class Tuple = std::tuple<T&&, Args&&...>,
      class Init = typename etude::unpacked<
        Tuple&&, etude::make_indices<etude::tuple_size<Tuple>::value - 1>
      >::type,
      class Last_ = typename etude::last_type<Args&&...>::type,
      class LastTuple = decltype( pack_t()( std::declval<Last_>() ) ),
      class Last = decltype( etude::unpack( std::declval<LastTuple>() ) ),
      class Result = decltype(
        etude::tuple_cat_( std::declval<Init>(), std::declval<Last>() )
      )
    >
    Result operator()( T && x, Args&&... args ) const {
      std::tuple<T&&, Args&&...> t( std::forward<T>(x), std::forward<Args>(args)... );
      auto lt = (*this)( etude::tuple_last( std::move(t) ) );
      return etude::tuple_cat_( Init( std::move(t) ), etude::unpack( std::move(lt) ) );
    }
    
  };
  
  namespace {
    pack_t const pack = {};
  }
  
} // namespace etude

#endif  // #ifndef ETUDE_FUNCTIONAL_INCLUDED_PACK_HPP_
