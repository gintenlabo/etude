//
//  greater:
//    std::greater の拡張版
// 
//    etude::greater<T, U> は std::greater<T> の上位互換の関数オブジェクトです。
// 
//    etude::greater<T, U> は、 T と U が > 演算子または < 演算子で比較可能な場合には、
//      bool operator()( T const&, U const& ) const;
//    という operator() が一つ定義され、
//    型 result_type や (first|second)_argument_type が適切に定義された、
//    空で trivial な関数オブジェクトになります。
//    比較不能の場合には、 operator() や typedef は定義されません。
//    
//    また、 T, U は省略可能であり、まず U のみが省略された場合、 U は T に定義されます。
//    この時 etude::greater<T> は、 T が比較可能な場合には、 std::greater<T> と同じです。
//    T, U が共に省略された場合、 etude::greater<> は、あらゆる比較可能な型の組み合わせ
//    T_, U_ に対して bool operator()( T_ const&, U_ const& ) const; の定義され、
//    型 result_type の定義された、空で trivial な関数オブジェクトになります。
//    この場合は (first|second)_argument_type は定義されません。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_FUNCTINAL_INCLUDED_GREATER_HPP_
#define ETUDE_FUNCTINAL_INCLUDED_GREATER_HPP_

#include <type_traits>
#include "../types/is_simply_callable.hpp"
#include "../utility/compare_greater.hpp"

namespace etude {
  
  // 実装
  template< class T, class U,
    class T_ = typename std::conditional<std::is_reference<T>::value, T, T const&>::type,
    class U_ = typename std::conditional<std::is_reference<U>::value, U, U const&>::type,
    class = void
  >
  struct greater_ {};
  
  template<class T, class U, class T_, class U_>
  struct greater_< T, U, T_, U_,
    typename std::enable_if<
      std::is_convertible<
        decltype(
          etude::compare_greater( std::declval<T_>(), std::declval<U_>() )
        ), bool
      >::value
    >::type
  >
  {
    typedef bool         result_type;
    typedef T    first_argument_type;
    typedef U   second_argument_type;
    
    bool operator()( T_ lhs, U_ rhs ) const {
      return etude::compare_greater( std::forward<T_>(lhs), std::forward<U_>(rhs) );
    }
    
  };
  
  
  // 本体（一般の場合）
  template<class T = void, class U = T>
  struct greater
    : greater_<T, U> {};
  
  // 型指定無しの場合
  template<>
  struct greater<>
  {
    typedef bool result_type;
    
    // 左右で同じ型を取る場合
    template< class T,
      class = typename std::enable_if<
        etude::is_simply_callable<
          etude::greater<T>, bool ( T const&, T const& )
        >::value
      >::type
    >
    bool operator()( T const& lhs, T const& rhs ) const {
      return etude::greater<T>()( lhs, rhs );
    }
    
    // 左右で異なる型を取る場合
    template< class T, class U,
      class = typename std::enable_if<
        etude::is_simply_callable<
          etude::greater<T, U>, bool ( T const&, U const& )
        >::value
      >::type
    >
    bool operator()( T const& lhs, U const& rhs ) const {
      return etude::greater<T, U>()( lhs, rhs );
    }
    
  };
  
  
  
} // namespace etude

#endif  // #ifndef ETUDE_FUNCTINAL_INCLUDED_GREATER_HPP_
