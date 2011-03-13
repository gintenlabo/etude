//
//  is_less_than_comparable :
//    == で比較できれば true になるメタ関数
//    
//    etude::is_less_than_comparable<T, U> は、
//    T const 型の変数 t および U const 型の変数 u に対して
//    t < u という式が well-formed で、かつその結果が bool に明示的に変換できるならば
//    std::true_type を、それ以外の場合には std::false_type を継承します。
//
//    なお、交換可能性や strict weak order になっているか否かは考慮されません。
//
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_TYPES_INCLUDED_IS_LESS_THAN_COMPARABLE_HPP_
#define ETUDE_TYPES_INCLUDED_IS_LESS_THAN_COMPARABLE_HPP_

#include <type_traits>
#include <utility>

namespace etude {

  // 実装用
  template<class T, class U, class = void>
  class is_less_than_comparable_
  {
    template< class T_ = T, class U_ = U,
      class R = decltype( std::declval<T_>() < std::declval<U_>() ),
      class = typename std::enable_if<std::is_constructible<bool, R>::value>::type
    >
    static std::true_type test_( int );
    
    static std::false_type test_( ... );
    
   public:
    typedef decltype( test_(0) ) type;
    
  };
  
  // gcc-4.5.0 では何故か比較できないポインタ同士が上記の SFINAE にひっかからない
  template<class T, class U>
  struct is_less_than_comparable_<T, U,
    typename std::enable_if<
      std::is_pointer<typename std::decay<T>::type>::value &&
      std::is_pointer<typename std::decay<U>::type>::value
    >::type
  >
  {
    typedef typename std::decay<T>::type T_;
    typedef typename std::decay<U>::type U_;
    
    typedef std::integral_constant< bool,
      std::is_convertible<T_, U_>::value || std::is_convertible<U_, T_>::value
    > type;
    
  };

  // 本体
  template<class T, class U = T>
  struct is_less_than_comparable :
    is_less_than_comparable_<
      typename std::conditional<std::is_reference<T>::value, T, T const&>::type,
      typename std::conditional<std::is_reference<U>::value, U, U const&>::type
    >::type
  {};

} // namespace etude

#endif  // #ifndef ETUDE_TYPES_INCLUDED_IS_LESS_THAN_COMPARABLE_HPP_
