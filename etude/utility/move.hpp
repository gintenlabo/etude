//
//  etude::move
//    constexpr version of std::move
//
//  Copyright (C) 2014  Takaya SAITO (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_UTILITY_INCLUDED_MOVE_HPP_
#define ETUDE_UTILITY_INCLUDED_MOVE_HPP_

#include <type_traits>

namespace etude {

// constexpr version of std::move
template<class T>
constexpr auto move(T&& x) noexcept
    -> typename std::remove_reference<T>::type&& {
  return static_cast<typename std::remove_reference<T>::type&&>(x);
}

}  // namespace etude

#endif  // #ifndef ETUDE_UTILITY_INCLUDED_MOVE_HPP_
