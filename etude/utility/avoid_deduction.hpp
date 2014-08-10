//
//  etude::avoid_deduction
//    Prohibits template deduction from argument type
//
//  Copyright (C) 2014  Takaya SAITO (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_UTILITY_INCLUDED_AVOID_DEDUCTION_HPP_
#define ETUDE_UTILITY_INCLUDED_AVOID_DEDUCTION_HPP_

namespace etude {

// class template to forbid template argument deduction
// definition is same to boost::identity<T> or std::enable_if<true, T>
template<class T>
struct avoid_deduction {
  typedef T type;
};

}  // namespace etude

#endif  // #ifndef ETUDE_UTILITY_INCLUDED_AVOID_DEDUCTION_HPP_
