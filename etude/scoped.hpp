//
//  scoped:
//    std::unique_ptr を作る
// 
//    簡単に std::unique_ptr を作るためのヘルパ関数です。
//    to_unique という名前でしたが、より短く分かりやすく。
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_INCLUDED_SCOPED_HPP_
#define ETUDE_INCLUDED_SCOPED_HPP_

#include <memory>
#include "types/decay_and_strip.hpp"

namespace etude {
  
  // simple wrapper functions for std::unique_ptr
  
  // ポインタとデリータから作る
  template< class T, class D = std::default_delete<T>,
    class D_ = typename decay_and_strip<D>::type >
  inline std::unique_ptr<T, D_> scoped( T* p, D d = D() ) {
    return std::unique_ptr<T, D_>( p, static_cast<D&&>(d) );
  }
  
  // デリータの種類を指定して作る
  template< class D, class T >
  inline std::unique_ptr<T, D> scoped( T* p ) {
    return std::unique_ptr<T, D>(p);
  }

} // namespace etude

#endif  // #ifndef ETUDE_INCLUDED_SCOPED_HPP_
