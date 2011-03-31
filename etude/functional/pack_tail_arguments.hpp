//
//  pack_tail_arguments:
//    最初以外の引数を tuple にまとめて返す
// 
//    与えられた引数が unpacked_tuple の場合は、その先頭の要素を返します。
//    そうでない場合は、与えられた最初の引数を返します。
//  
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_FUNCTIONAL_INCLUDED_PACK_TAIL_ARGUMENTS_HPP_
#define ETUDE_FUNCTIONAL_INCLUDED_PACK_TAIL_ARGUMENTS_HPP_

#include <tuple>

#include <utility>
#include <type_traits>

#include "unpacked_tuple.hpp"
#include "pack.hpp"
#include "tail_arguments.hpp"
#include "../types/is_unpacked_tuple.hpp"
#include "../types/tuple_element.hpp"

namespace etude {

  struct pack_tail_arguments_t
  {
    // 通常版
    template<class T, class... Args,
      class Result = decltype( etude::pack( std::declval<Args>()... ) ),
      class = typename std::enable_if<!is_unpacked_tuple<T>::value>::type
    >
    Result operator()( T&&, Args&&... args ) const {
      return etude::pack( std::forward<Args>(args)... );
    }
    
    // unpack
    template< class Unpacked,
      class Result = decltype(
        etude::pack( etude::tail_arguments( std::declval<Unpacked>() ) )
      ),
      class = typename std::enable_if<is_unpacked_tuple<Unpacked>::value>::type
    >
    Result operator()( Unpacked && t ) const
    {
      return etude::pack( etude::tail_arguments( std::declval<Unpacked>(t) ) );
    }
    
  };
  
  namespace {
    pack_tail_arguments_t const pack_tail_arguments = {};
  }

} // namespace etude

#endif  // #ifndef ETUDE_FUNCTIONAL_INCLUDED_PACK_TAIL_ARGUMENTS_HPP_
