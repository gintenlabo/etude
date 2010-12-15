//
//  make_unique:
//    メモリを確保して std::unique_ptr を作る
// 
//    メモリを確保して std::unique_ptr に格納する関数です。
//    new 演算子のより安全な代替として使えます。
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_INCLUDED_MAKE_UNIQUE_HPP_
#define ETUDE_INCLUDED_MAKE_UNIQUE_HPP_

#include <memory>

namespace etude {
  
  // 実装四行、用途無限大。
  template<class T, class... Args>
  inline std::unique_ptr<T> make_unique( Args&&... args ) {
    return std::unique_ptr<T>( new T( static_cast<Args&&>(args)... ) );
  }

} // namespace etude

#endif  // #ifndef ETUDE_INCLUDED_MAKE_UNIQUE_HPP_
