//
//  pipe_operator:
//    関数適用演算子 operator | の定義
// 
//    x | f は、 f(x) と同じです。
//    複数の引数を渡したい場合は、 group( x, y ) | f のように書くと、
//    f( x, y ) になります。
//  
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_FUNCTIONAL_INCLUDED_PIPE_OPERATOR_HPP_
#define ETUDE_FUNCTIONAL_INCLUDED_PIPE_OPERATOR_HPP_

#include <utility>
#include <type_traits>
#include "invoke.hpp"
#include "wrap_if_ptr_fn.hpp"

namespace etude {
  namespace pipe_operator {
  
    // unpack は invoke に既にある
    template<class T, class F,
      class Result = decltype( etude::invoke( std::declval<F>(), std::declval<T>() ) )
    >
    inline Result operator|( T && x, F && f )
    {
      return etude::invoke( std::forward<F>(f), std::forward<T>(x) );
    }
    
    // argument_type がある場合には型推論を助ける
    template<class F,
      class F_ = decltype( etude::wrap_if_ptr_fn( std::declval<F>() ) ),
      class T = typename std::decay<F_>::type::argument_type,
      class R = decltype( std::declval<F_>()( std::declval<T>() ) )
    >
    inline R operator|( typename std::add_rvalue_reference<T>::type x, F && f ) {
      return etude::wrap_if_ptr_fn( std::forward<F>(f) )( std::forward<T>(x) );
    }
    
  }  // namespace pipe_operator
} // namespace etude

#endif  // #ifndef ETUDE_FUNCTIONAL_INCLUDED_PIPE_OPERATOR_HPP_
