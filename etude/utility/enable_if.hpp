//
//  ETUDE_ENABLE_IF
//    SFINAE helper
//
//  Copyright (C) 2014  Takaya SAITO (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_UTILITY_INCLUDED_ENABLE_IF_HPP_
#define ETUDE_UTILITY_INCLUDED_ENABLE_IF_HPP_

#include <type_traits>

namespace etude {

struct enabler {
  typedef enabler& type;
  static enabler value;  // never defined

 private:
  enabler() {}  // forbid construction
  enabler(const enabler&) = delete;
  void operator=(enabler) = delete;
};

#define ETUDE_ENABLE_IF(...)                                      \
    typename ::std::enable_if<(__VA_ARGS__),                      \
        ::etude::enabler::type>::type = ::etude::enabler::value   \

}  // namespace etude

#endif  // #ifndef ETUDE_UTILITY_INCLUDED_ENABLE_IF_HPP_
