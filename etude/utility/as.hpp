//
//  etude::as
//    Performs implicit conversion
//
//  Copyright (C) 2014  Takaya SAITO (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_UTILITY_INCLUDED_AS_HPP_
#define ETUDE_UTILITY_INCLUDED_AS_HPP_

#include <utility>

#include "avoid_deduction.hpp"

namespace etude {

// helper function
// converts argument to type T implicitly and ignore result
template<class T>
void as_(typename avoid_deduction<T>::type) noexcept {
}

// Effective version; use perfect forward to minimize copy/move
// references are not handled well; temporary may be returned
template <class To, class From,
  typename std::enable_if<
      !std::is_reference<To>::value>::type* = nullptr,
  bool NoExcept = noexcept(etude::as_<To>(std::declval<From>()))
>
constexpr To as(From && from) noexcept(NoExcept) {
  return static_cast<From&&>(from);
}

// for references and expressions such as
//    - `etude::as<void*>(0)`
//    - `etude::as<std::vector<int>>({1, 2, 3})`
template<class To,
  bool NoExcept = noexcept(etude::as_<To>(std::declval<To>()))
  // same as std::is_nothrow_copy_constructible<To>::value,
  // except SFINAE is considered
>
constexpr To as(typename avoid_deduction<To&&>::type x)
    noexcept(NoExcept) {
  return static_cast<To&&>(x);
}

}  // namespace etude

#endif  // #ifndef ETUDE_UTILITY_INCLUDED_AS_HPP_
