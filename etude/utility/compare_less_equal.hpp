//
//  compare_less_equal:
//    与えられた引数に対し <= を適用する
// 
//    与えられた引数に対し <= 演算子を用いた比較を行い、
//    その結果を bool に明示的に変換して返します。
//  
//    ただし引数がポインタの場合や、与えられた引数に <= が定義されない場合は、
//    !( rhs < lhs ) を返します（ポインタ時には std::less になります）。
//  
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_UTILITY_INCLUDED_COMPARE_LESS_EQUAL_HPP_
#define ETUDE_UTILITY_INCLUDED_COMPARE_LESS_EQUAL_HPP_

#include <utility>
#include <type_traits>
#include "compare_less.hpp"

namespace etude {

  // 実装用
  
  // operator <= が定義されている、ただしポインタ以外の場合
  template< class T, class U,
    class = typename std::enable_if<
      !( std::is_pointer<T>::value && std::is_pointer<U>::value )
    >::type,
    class R = decltype(
      std::declval<T const&>() <= std::declval<U const&>()
    ),
    class = typename std::enable_if< std::is_constructible<bool, R>::value >::type
  >
  inline R compare_less_equal_impl_( T const& lhs, U const& rhs, int ) {
    return lhs <= rhs;
  }
  
  // operator <= が存在しない場合には compare_less_ に転送
  template< class T, class U,
    class R = decltype(
      !etude::compare_less_( std::declval<U const&>(), std::declval<T const&>() )
    ),
    class = typename std::enable_if< std::is_constructible<bool, R>::value >::type
  >
  inline R compare_less_equal_impl_( T const& lhs, U const& rhs, ... ) {
    return !etude::compare_less_( rhs, lhs );
  }
  
  
  // 本体
  
  
  // 戻り値を bool にキャストしない版
  template< class T, class U,
    class R = decltype(
      etude::compare_less_equal_impl_(
        std::declval<T const&>(), std::declval<U const&>(), 0
      )
    ),
    class = typename std::enable_if< std::is_constructible<bool, R>::value >::type
  >
  inline R compare_less_equal_( T const& lhs, U const& rhs ) {
    return compare_less_equal_impl_( lhs, rhs, 0 );
  }
  
  
  // 戻り値を bool にキャストする版
  
  // 同じ型の場合
  template< class T,
    class = decltype(
      etude::compare_less_equal_( std::declval<T const&>(), std::declval<T const&>() )
    )
  >
  inline bool compare_less_equal( T const& lhs, T const& rhs ) {
    return bool( compare_less_equal_( lhs, rhs ) );
  }
  // 異なる型の場合
  template< class T, class U,
    class = decltype(
      etude::compare_less_equal_( std::declval<T const&>(), std::declval<U const&>() )
    )
  >
  inline bool compare_less_equal( T const& lhs, U const& rhs ) {
    return bool( compare_less_equal_( lhs, rhs ) );
  }


} // namespace etude

#endif  // #ifndef ETUDE_UTILITY_INCLUDED_COMPARE_LESS_EQUAL_HPP_
