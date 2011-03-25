//
//  compare_not_equal_to:
//    与えられた引数に対し != を適用する
// 
//    与えられた引数に対し != 演算子を用いた比較ができる場合は、
//    その結果を bool に明示的に変換して返します。
//  
//    それ以外の場合、もし == 演算子による比較が出来る場合には、
//    その結果に対し ! 演算子を適用して返します。
//  
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_UTILITY_INCLUDED_COMPARE_NOT_EQUAL_TO_HPP_
#define ETUDE_UTILITY_INCLUDED_COMPARE_NOT_EQUAL_TO_HPP_

#include <utility>
#include <type_traits>
#include "compare_equal_to.hpp"

namespace etude {

  // 実装用
  
  // ポインタ以外で、 != による比較が可能な場合
  template< class T, class U,
    class = typename std::enable_if<
      !( std::is_pointer<T>::value && std::is_pointer<U>::value )
    >::type,
    class R = decltype( std::declval<T const&>() != std::declval<U const&>() ),
    class = typename std::enable_if< std::is_constructible<bool, R>::value >::type
  >
  inline R compare_not_equal_to_impl_( T const& lhs, U const& rhs, int ) {
    return lhs != rhs;
  }
  
  // compare_equal_to による比較が可能な場合
  template< class T, class U,
    class R = decltype(
      !etude::compare_equal_to_( std::declval<T const&>(), std::declval<U const&>() )
    ),
    class = typename std::enable_if< std::is_constructible<bool, R>::value >::type
  >
  inline R compare_not_equal_to_impl_( T const& lhs, U const& rhs, ... ) {
    return !etude::compare_equal_to_( lhs, rhs );
  }
  
  
  // 戻り値を bool にキャストしない版
  template< class T, class U,
    class R = decltype(
      etude::compare_not_equal_to_impl_(
        std::declval<T const&>(), std::declval<U const&>(), 0
      )
    ),
    class = typename std::enable_if< std::is_constructible<bool, R>::value >::type
  >
  inline R compare_not_equal_to_( T const& lhs, U const& rhs ) {
    return compare_not_equal_to_impl_( lhs, rhs, 0 );
  }
  
  
  // 戻り値を bool にキャストする版
  
  // 同じ型の場合
  template< class T,
    class = decltype(
      etude::compare_not_equal_to_( std::declval<T const&>(), std::declval<T const&>() )
    )
  >
  inline bool compare_not_equal_to( T const& lhs, T const& rhs ) {
    return bool( compare_not_equal_to_( lhs, rhs ) );
  }
  // 異なる型の場合
  template< class T, class U,
    class = decltype(
      etude::compare_not_equal_to_( std::declval<T const&>(), std::declval<U const&>() )
    )
  >
  inline bool compare_not_equal_to( T const& lhs, U const& rhs ) {
    return bool( compare_not_equal_to_( lhs, rhs ) );
  }


} // namespace etude

#endif  // #ifndef ETUDE_UTILITY_INCLUDED_COMPARE_NOT_EQUAL_TO_HPP_
