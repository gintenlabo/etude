//
//  init_of:
//    与えられた型リストのうち、最後の引数以外を返す
// 
//    etude::init_of<Ts...> は、
//    Ts... が Init... , Last と表せる場合は etude::types<Init...> を継承し、
//    Ts... が空の場合には空のクラスになります。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_TYPES_INCLUDED_INIT_OF_HPP_
#define ETUDE_TYPES_INCLUDED_INIT_OF_HPP_

#include "types.hpp"

namespace etude {
  
  template<class Types, class... Ts>
  struct init_of_ {};
  
  template<class... Ts, class T>
  struct init_of_<etude::types<Ts...>, T>
    : etude::types<Ts...> {};
  
  template<class... Ts, class T, class U, class... Us>
  struct init_of_<etude::types<Ts...>, T, U, Us...>
    : init_of_< etude::types<Ts..., T>, U, Us... >::type {};
  
  template<class... Types>
  struct init_of
    : init_of_<etude::types<>, Types...>{};

} // namespace etude

#endif  // #ifndef ETUDE_TYPES_INCLUDED_INIT_OF_HPP_
