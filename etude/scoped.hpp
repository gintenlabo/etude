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
//  このヘッダは http://www.rhinocerus.net/forum/language-c-moderated/
//    439657-c-0x-tuple-unpacking-arguments-function-call.html#post2038703
//  を参考にしています。
//
#ifndef ETUDE_INCLUDED_SCOPED_HPP_
#define ETUDE_INCLUDED_SCOPED_HPP_

#include <memory>
#include "types/decay_and_strip.hpp"

namespace etude {
  
  // simple wrapper function for std::unique_ptr
  template< class T, class D = std::default_delete<T>,
    class D_ = typename decay_and_strip<D>::type >
  inline std::unique_ptr<T, D_> scoped( T* p, D d = D() ) {
    return std::unique_ptr<T, D_>( p, static_cast<D&&>(d) );
  }
  
} // namespace etude

#endif  // #ifndef ETUDE_INCLUDED_SCOPED_HPP_
