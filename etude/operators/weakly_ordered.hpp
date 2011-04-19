//
//  weakly_ordered:
//    < 演算子から > 演算子を、 <= 演算子から >= 演算子を自動生成
// 
//    boost::less_than_comparable と違い、全順序を要求しません。
//  
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_OPERATORS_INCLUDED_WEAKLY_ORDERED_HPP_
#define ETUDE_OPERATORS_INCLUDED_WEAKLY_ORDERED_HPP_

#include <utility>
#include <type_traits>
#include "../utility/empty_base.hpp"

namespace etude {
 namespace weakly_ordered_ {
 
  template< class T, class Base = etude::empty_base<T> >
  struct weakly_ordered1
    : Base
  {
    // < から > の製作
    template< class T_ = T const&,
      class Result = decltype( std::declval<T_>() < std::declval<T_>() )
    >
    friend Result operator>( T const& lhs, T const& rhs ) {
      return rhs < lhs;
    }
    
    // <= から >= の製作
    template< class T_ = T const&,
      class Result = decltype( std::declval<T_>() <= std::declval<T_>() )
    >
    friend Result operator>=( T const& lhs, T const& rhs ) {
      return rhs <= lhs;
    }
    
  };
 
  template< class T, class U, class Base = etude::empty_base<T> >
  struct weakly_ordered2
    : Base
  {
    static_assert( !std::is_same<T const&, U const&>::value, "bad template arguments" );
    
    // 順序変更
    template< class T_ = T const&, class U_ = U const&,
      class Result = decltype( std::declval<T_>() > std::declval<U_>() )
    >
    friend Result operator<( U const& lhs, T const& rhs ) {
      return rhs > lhs;
    }
    template< class T_ = T const&, class U_ = U const&,
      class Result = decltype( std::declval<T_>() < std::declval<U_>() )
    >
    friend Result operator>( U const& lhs, T const& rhs ) {
      return rhs < lhs;
    }
    
    template< class T_ = T const&, class U_ = U const&,
      class Result = decltype( std::declval<T_>() >= std::declval<U_>() )
    >
    friend Result operator<=( U const& lhs, T const& rhs ) {
      return rhs >= lhs;
    }
    template< class T_ = T const&, class U_ = U const&,
      class Result = decltype( std::declval<T_>() <= std::declval<U_>() )
    >
    friend Result operator>=( U const& lhs, T const& rhs ) {
      return rhs <= lhs;
    }
    
  };
  
  // Boost.Operators のようなメタプログラミングは行わない
  template< class T, class U = T, class Base = etude::empty_base<T> >
  class weakly_ordered;
  
  template<class T, class Base>
  struct weakly_ordered<T, T, Base>
    : weakly_ordered1<T, Base> {};
  
  template<class T, class U, class Base>
  struct weakly_ordered
    : weakly_ordered2<T, U, Base> {};
  
 
 } // namespace weakly_ordered_
 using namespace weakly_ordered_;
 
} // namespace etude

#endif  // #ifndef ETUDE_OPERATORS_INCLUDED_WEAKLY_ORDERED_HPP_
