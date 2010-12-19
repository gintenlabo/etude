//
//  operator_new:
//    operator new を呼ぶ関数
// 
//    operator new を呼び、スマートポインタに入れて返します。
//    ::operator new を呼ぶ非テンプレート版と、
//    T::operator new があればそちらを呼ぶテンプレート版の二種類があります。
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_MEMORY_INCLUDED_OPERATOR_NEW_HPP_
#define ETUDE_MEMORY_INCLUDED_OPERATOR_NEW_HPP_

#include "default_deallocate.hpp"
#include "../types/type.hpp"
#include <memory>

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
  template<class T = void>
  struct raw_storage_pointer
  {
    typedef std::unique_ptr< void, etude::default_deallocate<T> > type;
  };
  
  template<class T, std::size_t N>
  struct raw_storage_pointer<T[N]>
  {
    typedef std::unique_ptr< void, etude::default_deallocate<T[]> > type;
  };
  
  template<>
  struct raw_storage_pointer<void>
  {
    typedef raw_storage_ptr type;
  };
  
  // 単独オブジェクト版
  // T::operator new があるなら
  template<class T,
    class = decltype( T::operator new( (std::size_t)0 ) )
  >
  inline void* operator_new_( std::size_t size, etude::type<T>, int ) {
    return T::operator new( size );
  }
  // T::operator new がないなら
  template<class T>
  inline void* operator_new_( std::size_t size, etude::type<T>, ... ) {
    return ::operator new( size );
  }
  
  // 配列版
  // T::operator new[] があるなら
  template<class T,
    class = decltype( T::operator new[]( (std::size_t)0 ) )
  >
  inline void* operator_new_( std::size_t size, etude::type<T[]>, int ) {
    return T::operator new[]( size );
  }
  // T::operator new[] がないなら
  template<class T>
  inline void* operator_new_( std::size_t size, etude::type<T[]>, ... ) {
    return ::operator new[]( size );
  }
  
  // 固定配列は配列に dispatch する
  template<class T, std::size_t N>
  inline void* operator_new_( std::size_t size, etude::type<T[N]>, ... ) {
    return operator_new_( size, etude::type<T[]>(), 0 );
  }
  
  
  // 本体
  template<class T,
    class Pointer = typename raw_storage_pointer<T>::type
  >
  inline Pointer operator_new( std::size_t size = sizeof(T) ) {
    return Pointer( etude::operator_new_( size, etude::type<T>(), 0 ) );
  }
  
  
} // namespace etude

#endif  // #ifndef ETUDE_MEMORY_INCLUDED_OPERATOR_NEW_HPP_