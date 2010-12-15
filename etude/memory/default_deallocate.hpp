//
//  default_deallocate:
//    operator delete を呼ぶファンクタ
// 
//    default_deallocate<T> は、
//    T::operator delete があるなら T::operator delete を、
//    ないならば ::operator delete を呼び出すファンクタです。
//    
//    T を U[] と書けば配列版の operator delete[] を呼び出せます。
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_MEMORY_INCLUDED_DEFAULT_DEALLOCATE_HPP_
#define ETUDE_MEMORY_INCLUDED_DEFAULT_DEALLOCATE_HPP_

#include <new>
#include "operator_delete.hpp"

namespace etude {
 namespace default_deallocate_ {  // 念のため ADL ガードを

  // 通常版
  template<class T=void>
  struct default_deallocate
  {
    typedef void result_type;
    
    void operator()( void* p ) const {
      return etude::operator_delete<T>(p);
    }
    
  };
  
  // 念のため void に対する特殊化をしておく
  template<>
  struct default_deallocate<void>
  {
    typedef void result_type;
    
    void operator()( void* p ) const {
      ::operator delete(p);
    }
    
  };
  

 }  // namespace default_deallocate_
 using namespace default_deallocate_;
} // namespace etude

#endif  // #ifndef ETUDE_MEMORY_INCLUDED_DEFAULT_DEALLOCATE_HPP_
