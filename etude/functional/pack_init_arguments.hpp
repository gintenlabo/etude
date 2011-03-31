//
//  pack_init_arguments:
//    最後以外の引数を tuple にまとめて返す
// 
//    与えられた引数が unpacked_tuple の場合は、その最後以外の要素を pack して返します。
//    そうでない場合は、与えられた最後以外の引数を pack して返します。
//  
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_UTILITY_INCLUDED_PACK_INIT_ARGUMENTS_HPP_
#define ETUDE_UTILITY_INCLUDED_PACK_INIT_ARGUMENTS_HPP_

#include <utility>
#include <type_traits>

#include "pack.hpp"
#include "../utility/tuple_init.hpp"

namespace etude {

  struct pack_init_arguments_t
  {
    // 通常版
    template< class... Args,
      class Result = decltype(
        etude::tuple_init(
          etude::pack( std::declval<Args>()... )
        )
      )
    >
    Result operator()( Args&&... args ) const {
      return etude::tuple_init( etude::pack( std::forward<Args>(args)... ) );
    }
    
  };
  
  namespace {
    pack_init_arguments_t const pack_init_arguments = {};
  }

} // namespace etude

#endif  // #ifndef ETUDE_UTILITY_INCLUDED_PACK_INIT_ARGUMENTS_HPP_
