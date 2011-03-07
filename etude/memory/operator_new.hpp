//
//  operator_new:
//    operator new を呼ぶ関数
// 
//    operator new を呼び、スマートポインタに入れて返します。
//    ::operator new を呼ぶ非テンプレート版と、
//    T::operator new があればそちらを呼ぶテンプレート版の二種類があります。
//    
//  Copyright (C) 2010-11  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_MEMORY_INCLUDED_OPERATOR_NEW_HPP_
#define ETUDE_MEMORY_INCLUDED_OPERATOR_NEW_HPP_

#include "default_deallocate.hpp"
#include "../types/types.hpp"
#include <memory>
#include <type_traits>

namespace etude {

  // 非テンプレート版
  
  // 戻り値の型
  typedef std::unique_ptr<void, etude::default_deallocate<>> raw_storage_ptr;
  // 本体
  inline raw_storage_ptr operator_new( std::size_t size ) {
    return raw_storage_ptr( ::operator new( size ) );
  }
  
  
  // テンプレート版
  
  // 戻り値を得るメタ関数
  // T が object-type ではない場合（関数、参照、 void の場合）には type は定義されない
  template<class T = void>
  struct raw_storage_pointer
    : std::enable_if<
        std::is_object<T>::value,
        std::unique_ptr< void, etude::default_deallocate<T> >
      >
  {};
  // void 版
  template<>
  struct raw_storage_pointer<void>
  {
    typedef raw_storage_ptr type;
  };
  
  
  // 単独オブジェクト版
  // T::operator new があるなら
  template<class T,
    class = typename std::enable_if<!std::is_array<T>::value>::type,
    class = decltype( T::operator new( (std::size_t)0 ) )
  >
  inline void* operator_new_( std::size_t size, int, int ) {
    return T::operator new( size );
  }
  // T::operator new がないなら
  template<class T,
    class = typename std::enable_if<!std::is_array<T>::value>::type
  >
  inline void* operator_new_( std::size_t size, int, ... ) {
    return ::operator new( size );
  }
  
  // 配列版
  // T::operator new[] があるなら
  template<class T,
    class = typename std::enable_if<std::is_array<T>::value>::type,
    class U = typename std::remove_all_extents<T>::type,
    class = decltype( U::operator new[]( (std::size_t)0 ) )
  >
  inline void* operator_new_( std::size_t size, void*, int ) {
    return U::operator new[]( size );
  }
  // T::operator new[] がないなら
  template<class T,
    class = typename std::enable_if<std::is_array<T>::value>::type
  >
  inline void* operator_new_( std::size_t size, void*, ... ) {
    return ::operator new[]( size );
  }
  
  
  // 本体
  
  // size を指定する基本版
  template<class T,
    class Pointer = typename raw_storage_pointer<T>::type
  >
  inline Pointer operator_new( std::size_t size ) {
    return Pointer( etude::operator_new_<T>( size, 0, 0 ) );
  }
  
  // size を推論させる場合（単独オブジェクトに限る）
  template<class T,
    class = typename std::enable_if<!std::is_array<T>::value>::type,
    class Pointer = typename raw_storage_pointer<T>::type,
    std::size_t Size = sizeof(T)
  >
  inline Pointer operator_new() {
    return Pointer( etude::operator_new_<T>( Size, 0, 0 ) );
  }
  
} // namespace etude

#endif  // #ifndef ETUDE_MEMORY_INCLUDED_OPERATOR_NEW_HPP_
