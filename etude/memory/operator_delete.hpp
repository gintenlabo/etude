//
//  operator_delete:
//    operator delete を呼ぶ関数
// 
//    operator_delete<T>(p) は、
//    T::operator delete があるなら T::operator delete(p) を、
//    ないならば ::operator delete(p) を呼び出します。
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_MEMORY_INCLUDED_OPERATOR_DELETE_HPP_
#define ETUDE_MEMORY_INCLUDED_OPERATOR_DELETE_HPP_

#include <new>

namespace etude {

  // 実装関数
  
  // T::operator delete があるならこっち
  template<class T,
    class = decltype( T::operator delete( (void*)0 ) )
  >
  inline void operator_delete_( void* p, int ) {
    T::operator delete(p);
  }
  
  // ないならこっち
  template<class T>
  inline void operator_delete_( void* p, ... ) {
    ::operator delete(p);
  }
  
  // 本体
  template<class T>
  inline void operator_delete( void* p ) {
    return etude::operator_delete_<T>( p, 0 );
  }
  
  
  // 配列版
  
  // T::operator delete [] があるならこっち
  template<class T,
    class = decltype( T::operator delete[]( (void*)0 ) )
  >
  inline void operator_array_delete_( void* p, int ) {
    T::operator delete [] (p);
  }
  
  // ないならこっち
  template<class T>
  inline void operator_array_delete_( void* p, ... ) {
    ::operator delete [] (p);
  }
  
  // 本体
  template<class T>
  inline void operator_array_delete( void* p ) {
    return etude::operator_array_delete_<T>( p, 0 );
  }
  
} // namespace etude

#endif  // #ifndef ETUDE_MEMORY_INCLUDED_OPERATOR_DELETE_HPP_
