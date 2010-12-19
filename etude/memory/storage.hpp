//
//  storage:
//    オブジェクトストレージ
// 
//    std::aligned_storage<sizeof(T), alignof(T)>::type を返すメタ関数としての機能と、
//    std::aligned_storage<sizeof(T), alignof(T)>::type 型のオブジェクトを
//    内部に実際に格納する noncopyable なバッファとしての機能を持ったクラスです。
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_MEMORY_INCLUDED_STORAGE_HPP_
#define ETUDE_MEMORY_INCLUDED_STORAGE_HPP_

#include "../noncopyable.hpp"
#include <type_traits>

namespace etude {

  // std::aligned_storage<sizeof(T), alignof(T)> に対する alias
  // 基本的には storage の方を使えばいいが、純粋なメタ関数も必要かも？
  template<class T>
  struct storage_of {
    typedef typename std::aligned_storage<sizeof(T), alignof(T)>::type type;
  };

  // ストレージ部分の実装
  template<class T, bool isEmpty = false>
  class storage_
    : private noncopyable
  {
    typename storage_of<T>::type buf_;
  };
  // empty class に対する最適化
  // 何も格納させない
  template<class T>
  class storage_<T, true>
    : private noncopyable
  {
    static_assert( std::is_empty<T>::value, "implementation error" );
  };
  
  // 本体
  template<class T>
  struct storage
    : private storage_<T, std::is_empty<T>::value>
  {
    // storage type
    typedef typename storage_of<T>::type type;
    
    // get address
    void* address() { return this; }
    void const* address() const { return this; }
  
  };
  
}

#endif  // #ifndef ETUDE_MEMORY_INCLUDED_STORAGE_HPP_
