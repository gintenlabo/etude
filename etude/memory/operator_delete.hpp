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
//  Copyright (C) 2010-11  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_MEMORY_INCLUDED_OPERATOR_DELETE_HPP_
#define ETUDE_MEMORY_INCLUDED_OPERATOR_DELETE_HPP_

#include <new>
#include <type_traits>

namespace etude {

  // 実装関数
  
  
  // 単独オブジェクト版
  
  // T::operator delete があるならこっち
  template<class T,
    class = typename std::enable_if<!std::is_array<T>::value>::type,
    class = decltype( T::operator delete( (void*)0 ) )
  >
  inline void operator_delete_( void* p, int, int ) {
    T::operator delete(p);
  }
  // ないならこっち
  template<class T,
    class = typename std::enable_if<!std::is_array<T>::value>::type
  >
  inline void operator_delete_( void* p, int, ... ) {
    ::operator delete(p);
  }
  
  
  // 配列版
  
  // T::operator delete [] があるならこっち
  template<class T,
    class = typename std::enable_if<std::is_array<T>::value>::type,
    class U = typename std::remove_all_extents<T>::type,
    class = decltype( U::operator delete[]( (void*)0 ) )
  >
  inline void operator_delete_( void* p, void*, int ) {
    U::operator delete [] (p);
  }
  // ないならこっち
  template<class T,
    class = typename std::enable_if<std::is_array<T>::value>::type
  >
  inline void operator_delete_( void* p, void*, ... ) {
    ::operator delete [] (p);
  }
  
  
  // 本体
  template<class T>
  inline void operator_delete( void* p ) {
    return etude::operator_delete_<T>( p, 0, 0 );
  }
  
} // namespace etude

#endif  // #ifndef ETUDE_MEMORY_INCLUDED_OPERATOR_DELETE_HPP_
