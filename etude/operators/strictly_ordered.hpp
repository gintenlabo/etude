//
//  strictly_ordered:
//    < 演算子から > , <= , >= 演算子を自動生成
// 
//    SFINAE する以外は boost::less_than_comparable とほぼ同じです。
//  
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_OPERATORS_INCLUDED_STRICTLY_ORDERED_HPP_
#define ETUDE_OPERATORS_INCLUDED_STRICTLY_ORDERED_HPP_

#include <utility>
#include <type_traits>
#include "../utility/empty_base.hpp"

#include "weakly_ordered.hpp"

namespace etude {
 namespace strictly_ordered_ {
 
  template< class T, class Base = etude::empty_base<T> >
  struct strictly_ordered1
    : weakly_ordered1< T, Base >
  {
    // < から <= の製作
    template< class T_ = T const&,
      class Result = decltype( !( std::declval<T_>() < std::declval<T_>() ) )
    >
    friend Result operator<=( T const& lhs, T const& rhs ) {
      return !( rhs < lhs );
    }
  
    // 向きの反転は weakly_ordered がやってくれる
    
  };
 
  template< class T, class U, class Base = etude::empty_base<T> >
  struct strictly_ordered2
    : weakly_ordered2< T, U, Base >
  {
    static_assert( !std::is_same<T const&, U const&>::value, "bad template arguments" );
    
    // > から <= の製作
    template< class T_ = T const&,
      class Result = decltype( !( std::declval<T_>() > std::declval<T_>() ) )
    >
    friend Result operator<=( T const& lhs, U const& rhs ) {
      return !( lhs > rhs );
    }
    // < から >= の製作
    template< class T_ = T const&,
      class Result = decltype( !( std::declval<T_>() < std::declval<T_>() ) )
    >
    friend Result operator>=( T const& lhs, U const& rhs ) {
      return !( lhs < rhs );
    }
  
    // 向きの反転は weakly_ordered がやってくれる
    
  };
  
  // Boost.Operators のようなメタプログラミングは行わない
  template< class T, class U = T, class Base = etude::empty_base<T> >
  class strictly_ordered;
  
  template<class T, class Base>
  struct strictly_ordered<T, T, Base>
    : strictly_ordered1<T, Base> {};
  
  template<class T, class U, class Base>
  struct strictly_ordered
    : strictly_ordered2<T, U, Base> {};
  
 
 } // namespace strictly_ordered_
 using namespace strictly_ordered_;
 
} // namespace etude

#endif  // #ifndef ETUDE_OPERATORS_INCLUDED_STRICTLY_ORDERED_HPP_
