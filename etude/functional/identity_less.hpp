//
//  identity_less:
//    etude::identity_before を呼び出す関数オブジェクト
// 
//    etude::identity_less<T, U> は、
//    etude::identity_before( T const volatile&, U const volatile& ) を呼び出す、
//    空で trivial な関数オブジェクトです。
//    
//    この関数オブジェクトは、比較が etude::identity_before になり、
//    (first|second)_argument_type が T const volatile& に定義される以外は、
//    etude::less と同様に定義されます。
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
#ifndef ETUDE_FUNCTINAL_INCLUDED_IDENTITY_LESS_HPP_
#define ETUDE_FUNCTINAL_INCLUDED_IDENTITY_LESS_HPP_

#include <type_traits>
#include <utility>
#include "../utility/identity_before.hpp"

namespace etude {
  
  template<class T = void, class U = T>
  class identity_less;
  
  template<>
  struct identity_less<>
  {
    typedef bool result_type;
    
    // etude::identity_before に転送
    template< class T, class U,
      class = decltype(
        etude::identity_before( std::declval<T&>(), std::declval<U&>() )
      )
    >
    bool operator()( T& lhs, U& rhs ) const {
      return etude::identity_before( lhs, rhs );
    }
    
  };
  
  
  template<class T, class U, class = void>
  struct identity_less_ {};
  
  template<class T, class U>
  struct identity_less_< T, U,
    typename std::enable_if<
      std::is_convertible<
        decltype(
          etude::identity_before( std::declval<T&>(), std::declval<U&>() )
        ), bool
      >::value
    >::type
  >
  {
    typedef bool result_type;
    typedef T&  first_argument_type;
    typedef U& second_argument_type;
    
    bool operator()( T& lhs, U& rhs ) const {
      return etude::identity_before( lhs, rhs );
    }
    
    // 一時オブジェクトは bind できないように
    typedef typename std::remove_reference<T>::type T_;
    typedef typename std::remove_reference<U>::type U_;
    void operator()( T_&&, U_&  ) const = delete;
    void operator()( T_&,  U_&& ) const = delete;
    void operator()( T_&&, U_&& ) const = delete;
    
  };
  
  template<class T, class U>
  struct identity_less
    : identity_less_<T const volatile&, U const volatile&> {};
  
} // namespace etude

#endif  // #ifndef ETUDE_FUNCTINAL_INCLUDED_IDENTITY_LESS_HPP_
