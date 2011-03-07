//
//  operator_arrow:
//    -> 演算子を呼び出す
// 
//    与えられた引数をコピーした prvalue を返します。
//    
//    この関数の戻り値の型は、引数を t としたとき、
//    auto x = t; によって作られる x と同じ型になります。
//  
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_UTILITY_INCLUDED_OPERATOR_ARROW_HPP_
#define ETUDE_UTILITY_INCLUDED_OPERATOR_ARROW_HPP_

#include <utility>
#include <type_traits>

namespace etude {

  template< class T,
    class = typename std::enable_if<
      std::is_class<T>::value || std::is_union<T>::value
    >::type
  >
  inline T* operator_arrow( T* x ) {
    return x;
  }

  template< class T,
    class Result = decltype( std::declval<T>().operator->() )
  >
  inline Result operator_arrow( T && x ) {
    return std::forward<T>(x).operator->();
  }

} // namespace etude

#endif  // #ifndef ETUDE_UTILITY_INCLUDED_OPERATOR_ARROW_HPP_
