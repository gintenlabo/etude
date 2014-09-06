//
//  etude::immutable
//    immutable object wrapper
//
//  Copyright (C) 2014  Takaya SAITO (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_UTILITY_INCLUDED_IMMUTABLE_HPP_
#define ETUDE_UTILITY_INCLUDED_IMMUTABLE_HPP_

#include <cassert>
#include <memory>
#include <utility>
#include <stdexcept>
#include "enable_if.hpp"

namespace etude {

struct in_place_t {
};
constexpr in_place_t in_place = {};

template<class T>
struct immutable {
  // create empty immutable object
  constexpr immutable() noexcept
      : p_(nullptr) {
  }

  // create immutable object
  template<class... Args>
  explicit immutable(in_place_t, Args&&... args)
      : p_(std::make_shared<T>(std::forward<Args>(args)...)) {
  }

  // accept shared_ptr (it should be unique)
  explicit immutable(std::shared_ptr<T> && p) {
    if (p && !p.unique()) {
      throw std::invalid_argument("shared_ptr should be unique");
    }
    p_ = std::move(p);
  }

  explicit operator bool() const noexcept {
    return p_.get() != nullptr;
  }
  bool unique() const noexcept {
    return p_.unique();
  }

  T const* get_ptr() const noexcept {
    return p_.get();
  }
  T const& operator*() const {
    assert(p_.get() != nullptr);
    return *p_;
  }
  T const* operator->() const {
    assert(p_.get() != nullptr);
    return p_.get();
  }

  std::shared_ptr<T const> get_shared() const noexcept {
    return p_;
  }

  void swap(immutable& x) noexcept {
    p_.swap(x.p_);
  }
  friend void swap(immutable& x, immutable& y) noexcept {
    x.swap(y);
  }

  // uniqify **this, and apply given function as mutable value
  template<class F>
  auto modify_with(F f)
      -> decltype(f(std::declval<T&>())) {
    assert(p_.get() != nullptr);

    if (!p_.unique()) {
      p_ = std::make_shared<T>(**this);
    }

    assert(p_.unique());
    return f(*p_);
  }

 private:
  std::shared_ptr<T> p_;
};

template<class T, class... Args>
immutable<T> make_immutable(Args&&... args) {
  return immutable<T>(in_place, std::forward<Args>(args)...);
}

}  // namespace etude

#endif  // #ifndef ETUDE_UTILITY_INCLUDED_IMMUTABLE_HPP_
