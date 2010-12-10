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

  template<class T, bool isEmpty = false>
  struct storage_
    : private noncopyable
  {
    // internal storage type
    typedef typename std::aligned_storage<sizeof(T), alignof(T)>::type storage_type;
    
    // get address
    void* address() { return &buf_; }
    void const* address() const { return &buf_; }
    
   private:
    storage_type buf_;
    
  };
  // empty class に対する最適化
  template<class T>
  struct storage_<T, true>
    : private noncopyable
  {
    static_assert( std::is_empty<T>::value, "implementation error" );
    // storage type (実際には格納しない)
    typedef typename std::aligned_storage<sizeof(T), alignof(T)>::type storage_type;
    // get address
    void* address() { return this; }
    void const* address() const { return this; }
  };
  
  // storage impl
  template<class T>
  class storage
    : private storage_<T, std::is_empty<T>::value>
  {
    typedef storage_<T, std::is_empty<T>::value> base;
    
   public:
    typedef typename base::storage_type type;
    using base::address;
  
  };
  
}

#endif  // #ifndef ETUDE_MEMORY_INCLUDED_STORAGE_HPP_
