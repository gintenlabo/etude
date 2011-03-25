//
//  identity_less_equal:
//    アドレスを <= で比較する関数オブジェクト
// 
//    etude::identity_less_equal<T, U> は、 T cv&, U cv& 型の引数 lhs, rhs に対し
//    !etude::identity_before( rhs, lhs ) を呼び出す、
//    空で trivial な関数オブジェクトです。
//    
//    この関数オブジェクトは、比較が etude::identity_before になり、
//    (first|second)_argument_type が T const volatile& に定義される以外は、
//    etude::less_equal と同様に定義されます。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_FUNCTINAL_INCLUDED_IDENTITY_LESS_EQUAL_HPP_
#define ETUDE_FUNCTINAL_INCLUDED_IDENTITY_LESS_EQUAL_HPP_

#include <type_traits>
#include <utility>
#include "../utility/identity_before.hpp"
#include "../types/is_simply_callable.hpp"

namespace etude {
  
  // 実装
  template<class T, class U, class = void>
  struct identity_less_equal_ {};
  
  template<class T, class U>
  struct identity_less_equal_< T, U,
    typename std::enable_if<
      std::is_convertible<
        decltype(
          !etude::identity_before( std::declval<U&>(), std::declval<T&>() )
        ), bool
      >::value
    >::type
  >
  {
    typedef bool result_type;
    typedef T&  first_argument_type;
    typedef U& second_argument_type;
    
    bool operator()( T& lhs, U& rhs ) const {
      return !etude::identity_before( rhs, lhs );
    }
    
    // 一時オブジェクトは bind できないように
    typedef typename std::remove_reference<T>::type T_;
    typedef typename std::remove_reference<U>::type U_;
    void operator()( T_&&, U_&  ) const = delete;
    void operator()( T_&,  U_&& ) const = delete;
    void operator()( T_&&, U_&& ) const = delete;
    
  };
  
  
  // 本体（一般の場合）
  template<class T = void, class U = T>
  struct identity_less_equal
    : identity_less_equal_<T const volatile&, U const volatile&> {};
  
  // 型指定無しの場合
  template<>
  struct identity_less_equal<>
  {
    typedef bool result_type;
    
    // etude::identity_less_equal<T&, U&> に転送
    template< class T, class U,
      class = typename std::enable_if<
        etude::is_simply_callable<
          etude::identity_less_equal<T&, U&>, bool ( T&, U& )
        >::value
      >::type
    >
    bool operator()( T& lhs, U& rhs ) const {
      return etude::identity_less_equal<T&, U&>()( lhs, rhs );
    }
    
  };
  
} // namespace etude

#endif  // #ifndef ETUDE_FUNCTINAL_INCLUDED_IDENTITY_LESS_EQUAL_HPP_
