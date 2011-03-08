//
//  equality_comparable:
//    == 演算子から != 演算子を自動生成
// 
//    SFINAE する以外は boost::equality_comparable とほぼ同じです。
//  
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_OPERATORS_INCLUDED_EQUALITY_COMPARABLE_HPP_
#define ETUDE_OPERATORS_INCLUDED_EQUALITY_COMPARABLE_HPP_

#include <utility>
#include <type_traits>
#include "../utility/empty_base.hpp"

namespace etude {
 namespace equality_comparable_ {
 
  template< class T, class Base = etude::empty_base<T> >
  struct equality_comparable1
    : Base
  {
    // == から != の製作
    template< class T_ = T const&,
      class Result = decltype( !( std::declval<T_>() == std::declval<T_>() ) )
    >
    friend Result operator!=( T const& lhs, T const& rhs ) {
      return !( lhs == rhs );
    }
    
  };
 
  template< class T, class U, class Base = etude::empty_base<T> >
  struct equality_comparable2
    : Base
  {
    static_assert( !std::is_same<T const&, U const&>::value, "bad template arguments" );
    
    // == から != の製作
    template< class T_ = T const&, class U_ = U const&,
      class Result = decltype( !( std::declval<T_>() == std::declval<U_>() ) )
    >
    friend Result operator!=( T const& lhs, U const& rhs ) {
      return !( lhs == rhs );
    }
    
    // 順序変更
    template< class T_ = T const&, class U_ = U const&,
      class Result = decltype( std::declval<T_>() == std::declval<U_>() )
    >
    friend Result operator==( U const& lhs, T const& rhs ) {
      return rhs == lhs;
    }
    template< class T_ = T const&, class U_ = U const&,
      class Result = decltype( !( std::declval<T_>() == std::declval<U_>() ) )
    >
    friend Result operator!=( U const& lhs, T const& rhs ) {
      return !( rhs == lhs );
    }
    
  };
  
  // Boost.Operators のようなメタプログラミングは行わない
  template< class T, class U = T, class Base = etude::empty_base<T> >
  class equality_comparable;
  
  template<class T, class Base>
  struct equality_comparable<T, T, Base>
    : equality_comparable1<T, Base> {};
  
  template<class T, class U, class Base>
  struct equality_comparable
    : equality_comparable2<T, U, Base> {};
  
 
 } // namespace equality_comparable_
 using namespace equality_comparable_;
 
} // namespace etude

#endif  // #ifndef ETUDE_OPERATORS_INCLUDED_EQUALITY_COMPARABLE_HPP_
