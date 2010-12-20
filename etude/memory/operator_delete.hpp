//
//  operator_delete:
//    operator delete を呼ぶ関数
// 
//    etude::operator_delete<T>(p) は、
//    T::operator delete があるなら T::operator delete(p) を、
//    ないならば ::operator delete(p) を呼び出します。
//    
//    etude::operator_delete<T[]>(p) と書くことで、配列にも対応可能。
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_MEMORY_INCLUDED_OPERATOR_DELETE_HPP_
#define ETUDE_MEMORY_INCLUDED_OPERATOR_DELETE_HPP_

#include <new>
#include "../types/types.hpp"

namespace etude {

  // 実装関数
  
  
  // 単独オブジェクト版
  
  // T::operator delete があるならこっち
  template<class T,
    class = decltype( T::operator delete( (void*)0 ) )
  >
  inline void operator_delete_( void* p, etude::type<T>, int ) {
    T::operator delete(p);
  }
  // ないならこっち
  template<class T>
  inline void operator_delete_( void* p, etude::type<T>, ... ) {
    ::operator delete(p);
  }
  
  
  // 配列版
  
  // T::operator delete [] があるならこっち
  template<class T,
    class = decltype( T::operator delete[]( (void*)0 ) )
  >
  inline void operator_delete_( void* p, etude::type<T[]>, int ) {
    T::operator delete [] (p);
  }
  // ないならこっち
  template<class T>
  inline void operator_delete_( void* p, etude::type<T[]>, ... ) {
    ::operator delete [] (p);
  }
  
  // 固定配列は ill-formed
  template<class T, std::size_t N>
  inline void operator_delete_( void* p, etude::type<T[N]>, ... ) {
    static_assert( N < 0, "etude::operator_delete<T[N]> is ill-formed." );
  }
  
  
  // 本体
  template<class T>
  inline void operator_delete( void* p ) {
    return etude::operator_delete_( p, etude::type<T>(), 0 );
  }
  
} // namespace etude

#endif  // #ifndef ETUDE_MEMORY_INCLUDED_OPERATOR_DELETE_HPP_
