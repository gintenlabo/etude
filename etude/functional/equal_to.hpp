//
//  equal_to:
//    std::equal_to の拡張版
// 
//    etude::equal_to<T, U> は std::equal_to<T> の上位互換です。
// 
//    etude::equal_to<T, U> は、 T と U が == 演算子で比較可能な場合には、
//      bool operator()( T const&, U const& ) const;
//    という operator() が一つ定義され、
//    型 result_type や (first|second)_argument_type が適切に定義された、
//    空で trivial な関数オブジェクトになります。
//    比較不能の場合には、 operator() や typedef は定義されません。
//    
//    また、 T, U は省略可能であり、まず U のみが省略された場合、 U は T に定義されます。
//    この時、 etude::equal_to<T> は、 T が比較可能ならば std::equal_to<T> と同じです。
//    T, U が共に省略された場合、 etude::equal_to<> は、あらゆる比較可能な型の組み合わせ
//    T_, U_ に対して bool operator()( T_ const&, U_ const& ) const; の定義され、
//    型 result_type の定義された、空で trivial な関数オブジェクトになります。
//    この場合は (first|second)_argument_type は定義されません。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_FUNCTINAL_INCLUDED_EQUAL_TO_HPP_
#define ETUDE_FUNCTINAL_INCLUDED_EQUAL_TO_HPP_

#include <type_traits>
#include "../types/is_equality_comparable.hpp"
#include "../types/is_simply_callable.hpp"

namespace etude {
  
  // 実装用
  template<class T, class U, class = void>
  struct equal_to_ {};
  
  template<class T, class U>
  struct equal_to_< T, U,
    typename std::enable_if<
      etude::is_equality_comparable<T const&, U const&>::value
    >::type
  >
  {
    typedef bool         result_type;
    typedef T    first_argument_type;
    typedef U   second_argument_type;
    
    bool operator()( T const& lhs, U const& rhs ) const {
      return bool( lhs == rhs );
    }
    
  };
  
  // 本体（一般の場合）
  template<class T = void, class U = T>
  struct equal_to
    : equal_to_<T, U> {};
  
  // 型指定無しの場合
  template<>
  struct equal_to<>
  {
    typedef bool result_type;
    
    // 左右で同じ型を取る場合
    template< class T,
      class = typename std::enable_if<
        etude::is_simply_callable<
          etude::equal_to<T>, bool ( T const&, T const& )
        >::value
      >::type
    >
    bool operator()( T const& lhs, T const& rhs ) const {
      return etude::equal_to<T>()( lhs, rhs );
    }
    
    // 左右で異なる型を取る場合
    template< class T, class U,
      class = typename std::enable_if<
        etude::is_simply_callable<
          etude::equal_to<T, U>, bool ( T const&, U const& )
        >::value
      >::type
    >
    bool operator()( T const& lhs, U const& rhs ) const {
      return etude::equal_to<T, U>()( lhs, rhs );
    }
    
  };
  
  
} // namespace etude

#endif  // #ifndef ETUDE_FUNCTINAL_INCLUDED_EQUAL_TO_HPP_
