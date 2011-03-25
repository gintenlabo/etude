//
//  compare_less:
//    与えられた引数に対し < を適用する
// 
//    与えられた引数に対し < 演算子を用いた比較を行い、
//    その結果を bool に明示的に変換して返します。
//  
//    ただし引数がポインタの場合には、 std::less を使って比較します。
//  
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_UTILITY_INCLUDED_COMPARE_LESS_HPP_
#define ETUDE_UTILITY_INCLUDED_COMPARE_LESS_HPP_

#include <utility>
#include <type_traits>
#include "less_pointer.hpp"

namespace etude {

  // 実装用
  
  // ポインタ
  template< class T, class U,
    class R = decltype(
      etude::less_pointer( std::declval<T const&>(), std::declval<U const&>() )
    )
  >
  inline R compare_less_impl_( T const& lhs, U const& rhs, int ) {
    return etude::less_pointer( lhs, rhs );
  }
  
  // ポインタ以外
  template< class T, class U,
    class = typename std::enable_if<
      !( std::is_pointer<T>::value && std::is_pointer<U>::value )
    >::type,
    class R = decltype(
      std::declval<T const&>() < std::declval<U const&>()
    ),
    class = typename std::enable_if< std::is_constructible<bool, R>::value >::type
  >
  inline R compare_less_impl_( T const& lhs, U const& rhs, ... ) {
    return lhs < rhs;
  }
  
  
  // 本体
  
  
  // 戻り値を bool にキャストしない版
  template< class T, class U,
    class R = decltype(
      etude::compare_less_impl_(
        std::declval<T const&>(), std::declval<U const&>(), 0
      )
    ),
    class = typename std::enable_if< std::is_constructible<bool, R>::value >::type
  >
  inline R compare_less_( T const& lhs, U const& rhs ) {
    return compare_less_impl_( lhs, rhs, 0 );
  }
  
  
  // 戻り値を bool にキャストする版
  
  // 同じ型の場合
  template< class T,
    class = decltype(
      etude::compare_less_( std::declval<T const&>(), std::declval<T const&>() )
    )
  >
  inline bool compare_less( T const& lhs, T const& rhs ) {
    return bool( compare_less_( lhs, rhs ) );
  }
  // 異なる型の場合
  template< class T, class U,
    class = decltype(
      etude::compare_less_( std::declval<T const&>(), std::declval<U const&>() )
    )
  >
  inline bool compare_less( T const& lhs, U const& rhs ) {
    return bool( compare_less_( lhs, rhs ) );
  }


} // namespace etude

#endif  // #ifndef ETUDE_UTILITY_INCLUDED_COMPARE_LESS_HPP_
