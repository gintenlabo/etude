//
//  etude::move, etude::forward
//    constexpr versions of std::move and std::forward
//
//  Copyright (C) 2014  Takaya SAITO (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_UTILITY_INCLUDED_FORWARD_HPP_
#define ETUDE_UTILITY_INCLUDED_FORWARD_HPP_

#include <type_traits>
#include "avoid_deduction.hpp"

namespace etude {

// constexpr version of std::move
// unlike std::move, rvalue references are not supported
// (you should use this function iff you are moving lvalues)
template<class T>
constexpr T&& move(T& x) noexcept {
  return static_cast<T&&>(x);
}

// constexpr version of std::forward
// unlike std::forward, rvalue reference version is deleted for all type
// (you should use this function iff you are forwarding lvalues)
template<class T>
constexpr T&& forward(typename avoid_deduction<T&>::type x) noexcept {
  return static_cast<T&&>(x);
}
template<class T>
void forward(typename std::remove_reference<T>::type&& x) = delete;

// simple macro for generic lambdas
// var should be id-expression
#define ETUDE_FORWARD(var) ::etude::forward<decltype(var)>(var)

}  // namespace etude

#endif  // #ifndef ETUDE_UTILITY_INCLUDED_FORWARD_HPP_
