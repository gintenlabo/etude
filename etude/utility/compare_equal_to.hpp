//
//  compare_equal_to:
//    与えられた引数に対し == を適用する
// 
//    与えられた引数を == で比較し、その結果を bool に明示的に変換します。
//  
//  
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_UTILITY_INCLUDED_COMPARE_EQUAL_TO_HPP_
#define ETUDE_UTILITY_INCLUDED_COMPARE_EQUAL_TO_HPP_

#include <utility>
#include <type_traits>
#include "../types/is_equality_comparable.hpp"

namespace etude {

  // 戻り値を bool にキャストしない版
  template< class T, class U,
    class R = decltype(
      std::declval<T const&>() == std::declval<U const&>()
    ),
    class = typename std::enable_if<
      etude::is_equality_comparable<T, U>::value
    >::type
  >
  inline R compare_equal_to_( T const& lhs, U const& rhs ) {
    return lhs == rhs;
  }


  // 戻り値を bool にキャストする版
  
  // 同じ型の場合
  template< class T,
    class = typename std::enable_if<
      etude::is_equality_comparable<T>::value
    >::type
  >
  inline bool compare_equal_to( T const& lhs, T const& rhs ) {
    return bool( lhs == rhs );
  }
  // 異なる型の場合
  template< class T, class U,
    class = typename std::enable_if<
      etude::is_equality_comparable<T, U>::value
    >::type
  >
  inline bool compare_equal_to( T const& lhs, U const& rhs ) {
    return bool( lhs == rhs );
  }

} // namespace etude

#endif  // #ifndef ETUDE_UTILITY_INCLUDED_COMPARE_EQUAL_TO_HPP_
