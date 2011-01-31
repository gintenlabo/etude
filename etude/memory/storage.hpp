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

#include <type_traits>
#include "../types/storage_size.hpp"
#include "../types/storage_align.hpp"

namespace etude {

  template<class... Types>
  struct storage_of_;
  
  template<>
  struct storage_of_<>
  {
    static const std::size_t size  = 1;
    static const std::size_t align = 1;
    
    static const bool is_empty = true;
    
  };
  
  template<class T, class... Types>
  class storage_of_<T, Types...>
  {
    typedef storage_of_<Types...> tail;
    static const std::size_t s1 = storage_size<T>::value,  s2 = tail::size;
    static const std::size_t a1 = storage_align<T>::value, a2 = tail::align;
    
    static_assert( s1 != 0 && a1 != 0, "T must be object or reference." );
    
   public:
    static const std::size_t size  = (s1>s2) ? s1 : s2;
    static const std::size_t align = (a1>a2) ? a1 : a2;
    
    static const bool is_empty = tail::is_empty && std::is_empty<T>::value;
    
  };
  
  // 与えられた型全てを格納できるストレージクラス
  template<class... Ts>
  struct storage_of
  {
    static const std::size_t size  = storage_of_<Ts...>::size;
    static const std::size_t align = storage_of_<Ts...>::align;
    
    static const bool is_empty = storage_of_<Ts...>::is_empty;
    
    typedef typename std::aligned_storage<size, align>::type type;
    
  };

  // ストレージ部分の実装
  template<class, class... Ts>
  class storage_
  {
    typename storage_of<Ts...>::type buf_;
    
   public:
    storage_() = default;
    
    storage_( storage_ const& ) = delete;
    void operator=( storage_ const& ) = delete;
    
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
    
    storage_( storage_ const& ) = delete;
    void operator=( storage_ const& ) = delete;
    
  };
  
  // 本体
  template<class... Ts>
  struct storage
    : private storage_<void, Ts...>
  {
    // storage type
    typedef typename storage_of<Ts...>::type type;
    
    // get address
    void* address() { return this; }
    void const* address() const { return this; }
  
  };
  
}

#endif  // #ifndef ETUDE_MEMORY_INCLUDED_STORAGE_HPP_
