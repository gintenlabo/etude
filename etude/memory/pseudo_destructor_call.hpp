//
//  pseudo_destructor_call:
//    デストラクタ呼び出しを行うファンクタ
// 
//    p->~T(); あるいは x.~T(); 呼び出しを行います。
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_MEMORY_INCLUDED_PSEUDO_DESTRUCTOR_CALL_HPP_
#define ETUDE_MEMORY_INCLUDED_PSEUDO_DESTRUCTOR_CALL_HPP_


namespace etude {

  template<class T>
  struct pseudo_destructor_call
  {
    typedef void result_type;
    
    void operator()( T* p ) const {
      p->~T();
    }
    
    void operator()( T& x ) const {
      x.~T();
    }
    
  };

}

#endif  // #ifndef ETUDE_MEMORY_INCLUDED_PSEUDO_DESTRUCTOR_CALL_HPP_
