//
//  is_convertible :
//    std::is_convertible の改良版
//    
//    etude::types を使うことで、複数の型を同時に比較できるように。
//
//  Copyright (C) 2010-11  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_TYPES_INCLUDED_IS_CONVERTIBLE_HPP_
#define ETUDE_TYPES_INCLUDED_IS_CONVERTIBLE_HPP_

#include "types.hpp"
#include "bool_constant.hpp"
#include "is_constructible.hpp"
#include <type_traits>

namespace etude {

  // 基本的には std::is_convertible に転送
  template<class From, class To>
  struct is_convertible
    : std::is_convertible<From, To> {};
  
  
  // etude::types の場合のみ特別扱い
  
  // 双方が types の場合は、対応する型が convertible か
  template<class T1, class... Ts, class U1, class... Us>
  struct is_convertible< etude::types<T1, Ts...>, etude::types<U1, Us...> >
    : etude::bool_constant<
        etude::is_convertible<T1, U1>::value &&
        etude::is_convertible<etude::types<Ts...>, etude::types<Us...>>::value
      >::type
  {};
  
  template<class... Args>
  struct is_convertible<etude::types<Args...>, etude::types<>>
    : etude::bool_constant<( sizeof...(Args) == 0 )>::type {};
  template<class... Args>
  struct is_convertible<etude::types<>, etude::types<Args...>>
    : etude::bool_constant<( sizeof...(Args) == 0 )>::type {};
  template<>
  struct is_convertible<etude::types<>, etude::types<>>
    : std::true_type {};
  
  // From が types で To が普通の型の場合は、 is_constructible に転送
  template<class To, class... Args>
  struct is_convertible< etude::types<Args...>, To >
    : etude::is_constructible<To, Args...> {};

} // namespace etude

#endif  // #ifndef ETUDE_TYPES_INCLUDED_IS_CONVERTIBLE_HPP_
