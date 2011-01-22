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
#include <utility>
#include <type_traits>
#include "types/decay_and_strip.hpp"

namespace etude {
  
  // simple wrapper functions for std::unique_ptr
  
  // デフォルト版
  template<class T>
  inline std::unique_ptr<T> scoped( T* p ) {
    return std::unique_ptr<T>(p);
  }
  
  // デリータの種類を指定して作る
  template<class D, class T>
  inline std::unique_ptr<T, D> scoped( T* p ) {
    return std::unique_ptr<T, D>(p);
  }
  
  // ポインタとデリータから作る
  template< class T, class D,
    class D_ = typename decay_and_strip<D>::type
  >
  inline std::unique_ptr<T, D_> scoped( T* p, D d ) {
    return std::unique_ptr<T, D_>( p, std::forward<D>(d) );
  }
  
  
  // D に pointer があり、かつそれがポインタではない場合はこちらが使われる
  
  // デリータの種類を指定して作る
  template< class D, class P,
    class = typename std::enable_if<
      !std::is_pointer<
        typename std::remove_reference<D>::type::pointer
      >::value
    >::type
  >
  inline std::unique_ptr<void, D> scoped( P && p ) {
    return std::unique_ptr<void, D>( std::forward<P>(p) );
  }
  
  // デリータとポインタから作る
  template< class P, class D,
    class D_ = typename decay_and_strip<D>::type,
    class = typename std::enable_if<
      !std::is_pointer<
        typename std::remove_reference<D_>::type::pointer
      >::value
    >::type
  >
  inline std::unique_ptr<void, D_> scoped( P && p, D && d ) {
    return std::unique_ptr<void, D_>( std::forward<P>(p), std::forward<D>(d) );
  }

} // namespace etude

#endif  // #ifndef ETUDE_INCLUDED_SCOPED_HPP_
