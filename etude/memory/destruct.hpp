//
//  destruct:
//    メモリを解放せずオブジェクトを破棄する関数（安全版）
// 
//    std::unique_ptr<T> を受け取り、デストラクタを呼び出してオブジェクトを破棄した後、
//    std::unique_ptr<void, default_deallocate<T>> に抜け殻を格納して返す関数です。
//    
//    要するに construct の反対の動作を行います。
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_MEMORY_INCLUDED_DESTRUCT_HPP_
#define ETUDE_MEMORY_INCLUDED_DESTRUCT_HPP_

#include "construct.hpp"
#include <memory>

namespace etude {

  template<class T,
    class Result = std::unique_ptr< void, default_deallocate<T> >
  >
  inline Result destruct( std::unique_ptr<T> && p )
  {
    p->~T();
    void* const vp = p.release();
    return Result( vp );
  }
  
  // construct の結果として作られたポインタの場合
  template<class T, class Dealloc,
    class Result = std::unique_ptr<void, Dealloc>
  >
  inline Result destruct( std::unique_ptr<T, dispose_and_deallocate_<T, Dealloc>> && p )
  {
    p->~T();
    void* const vp = p.release();
    return Result( vp, std::forward<Dealloc>( p.get_deleter().get_deallocator() ) );
  }
  
} // namespace etude

#endif  // #ifndef ETUDE_MEMORY_INCLUDED_DESTRUCT_HPP_
