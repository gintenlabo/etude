//
//  tuple_size :
//    std::tuple_size の参照対応版
//    
//    perfect-forward やってると、 tuple を参照で受けることも多いので、
//    その場合にいちいち remove_reference するのが面倒で作ったもの。
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_TYPES_INCLUDED_TUPLE_SIZE_HPP_
#define ETUDE_TYPES_INCLUDED_TUPLE_SIZE_HPP_

#include <type_traits>
#include <tuple>

namespace etude {

  // 参照と CV 修飾子を消し飛ばして tuple_size を適用
  template<class T>
  struct tuple_size
    : std::tuple_size<typename std::decay<T>::type> {};

} // namespace etude

#endif  // #ifndef ETUDE_TYPES_INCLUDED_TUPLE_SIZE_HPP_
