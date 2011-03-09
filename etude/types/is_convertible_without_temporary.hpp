//
//  is_convertible_without_temporary :
//    std::is_convertible の改良版
//    
//    etude::is_convertible_without_temporary<From, To> は、
//      std::is_convertible<From, To>::value が false の場合には
//    std::false_type を継承します。
//
//    それ以外の場合、型 To が参照でない場合には
//      std::is_convertible<From, To>::type
//    を、型 To が型 T に対する non-volatile const lvalue reference である場合には
//      std::is_convertible<From&, T const volatile&>::type
//    を、型 To がそれ以外の lvalue reference である場合には
//      std::is_convertible<From, To>::type
//    を、型 To が rvalue reference である場合には
//      etude::is_convertible_without_temporary<From&, To&>::type
//    を、それぞれ継承します。
//
//    [ note:
//      これにより、一時オブジェクトを介する型変換を抑止しつつ、
//      それ以外の型変換は可能なかぎり許した型変換可能性を調査することができます。
//    ]
//
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_TYPES_INCLUDED_IS_CONVERTIBLE_WITHOUT_TEMPORARY_HPP_
#define ETUDE_TYPES_INCLUDED_IS_CONVERTIBLE_WITHOUT_TEMPORARY_HPP_

#include <type_traits>

namespace etude {

  template<class From, class To>
  class is_convertible_without_temporary;
  
  
  template<class From, class To, class = void>
  struct is_convertible_without_temporary_
    : std::is_convertible<From, To>::type {};
  
  template<class From, class To>
  struct is_convertible_without_temporary_< From, To const&,
    typename std::enable_if<
      std::is_convertible<From, To const&>::value &&
      !std::is_volatile<To>::value
    >::type
  >
    : std::is_convertible<From&, To const volatile&>::type {};
  
  template<class From, class To>
  struct is_convertible_without_temporary_< From, To&&,
    typename std::enable_if<
      std::is_convertible<From, To&&>::value
    >::type
  >
    : etude::is_convertible_without_temporary<From&, To&>::type {};

  template<class From, class To>
  struct is_convertible_without_temporary
    : is_convertible_without_temporary_<From, To>::type {};

} // namespace etude

#endif  // #ifndef ETUDE_TYPES_INCLUDED_IS_CONVERTIBLE_WITHOUT_TEMPORARY_HPP_
