//
//  storage:
//    オブジェクトストレージ
// 
//    std::aligned_storage<sizeof(T), alignof(T)>::type を返すメタ関数としての機能と、
//    std::aligned_storage<sizeof(T), alignof(T)>::type 型のオブジェクトを
//    内部に実際に格納する noncopyable なバッファとしての機能を持ったクラスです。
//    
//  Copyright (C) 2010-11  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_MEMORY_INCLUDED_STORAGE_HPP_
#define ETUDE_MEMORY_INCLUDED_STORAGE_HPP_

#include <type_traits>
#include "../types/storage_of.hpp"
#include "../utility/uninitialized.hpp"

namespace etude {

  // ストレージ部分の実装
  template<class, class... Ts>
  class storage_
  {
    typename storage_of<Ts...>::type buf_;
    
   public:
    storage_() = default;
    storage_( etude::uninitialized_t ) {}
    
  };
  // empty class に対する最適化
  // 何も格納させない
  template<class... Ts>
  struct storage_<
    typename std::enable_if< storage_of<Ts...>::is_empty >::type,
    Ts...
  >
  {
    storage_() = default;
    storage_( etude::uninitialized_t ) {}
    
  };
  
  // 本体
  template<class... Ts>
  struct storage
    : private storage_<void, Ts...>
  {
    // storage type
    typedef typename storage_of<Ts...>::type type;
    
    // construct
    storage() = default;
    // 明示的に初期化しない旨を分かりやすく
    storage( etude::uninitialized_t )
      : storage_<void, Ts...>( etude::uninitialized ) {}
    
    // noncopyable
    storage( storage const& ) = delete;
    void operator=( storage const& ) = delete;
    
    // get address
    void* address() { return this; }
    void const* address() const { return this; }
  
  };
  
}

#endif  // #ifndef ETUDE_MEMORY_INCLUDED_STORAGE_HPP_
