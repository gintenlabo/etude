//
//  init_arguments:
//    最後以外の引数を unpacked_tuple にまとめて返す
// 
//    与えられた引数が unpacked_tuple の場合は、その最後以外の要素を返します。
//    そうでない場合は、与えられた最後以外の引数を返します。
//  
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_UTILITY_INCLUDED_INIT_ARGUMENTS_HPP_
#define ETUDE_UTILITY_INCLUDED_INIT_ARGUMENTS_HPP_

#include <utility>
#include <type_traits>

#include "unpack.hpp"
#include "pack.hpp"
#include "../types/is_unpacked_tuple.hpp"
#include "../types/tuple_size.hpp"
#include "../types/indices.hpp"

namespace etude {

  struct init_arguments_t
  {
    // 通常版
    template< class... Args,
      class Pack = decltype(
        etude::pack( std::declval<Args>()... )
      ),
      class Result = typename etude::unpacked<
        Pack, etude::make_indices<etude::tuple_size<Pack>::value - 1>
      >::type
    >
    Result operator()( Args&&... args ) const {
      return Result( etude::pack( std::forward<Args>(args)... ) );
    }
    
  };
  
  namespace {
    init_arguments_t const init_arguments = {};
  }

} // namespace etude

#endif  // #ifndef ETUDE_UTILITY_INCLUDED_INIT_ARGUMENTS_HPP_
