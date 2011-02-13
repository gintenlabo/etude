//
//  ptr_fn:
//    std::mem_fn の関数ポインタ版
// 
//    与えられたシグネチャ（関数形式）に対応する各種 typedef を提供します。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_FUNCTINAL_INCLUDED_PTR_FN_HPP_
#define ETUDE_FUNCTINAL_INCLUDED_PTR_FN_HPP_

#include "function_base.hpp"
#include <utility>

namespace etude {
  
  template<class Singature>
  class pointer_to_function;
  
  // 関数ポインタへのラッパークラス
  template<class R, class... Args>
  struct pointer_to_function<R (Args...)>
    : function_base< R (Args...), pointer_to_function<R (Args...)> >
  {
    typedef R (*pointer)( Args... );
    
    pointer_to_function( pointer pf_ )
      : pf( pf_ ) {}
    
    // 基本的にはこちらが呼ばれる
    template<class... As,
      class = decltype( std::declval<pointer>()( std::declval<As>()... ) )
    >
    R operator()( As&&... as ) const {
      return (*pf)( std::forward<As>(as)... );
    }
    
    // ただし 0 を void* に変換したい場合などのためにこちらも必要
    R operator()( Args... args ) const {
      return (*pf)( std::forward<Args>(args)... );
    }
    
    // 内部のポインタを取得する
    pointer get() const { return pf; }
    
   private:
    pointer pf;
  };
  // ヘルパ関数
  template<class R, class... Args>
  inline pointer_to_function<R (Args...)> ptr_fn( R (*f)(Args...) ) {
    return f;
  }
  
  // ... がある場合に対しても一応対応させる
  template<class R, class... Args>
  struct pointer_to_function<R (Args..., ...)>
    : function_base< R (Args..., ...), pointer_to_function<R (Args..., ...)> >
  {
    typedef R (*pointer)( Args..., ... );
    
    pointer_to_function( pointer pf_ )
      : pf( pf_ ) {}
    
    // ... がある場合は専らこちら。 void* とか気にしない。
    template<class... As,
      class = decltype( std::declval<pointer>()( std::declval<As>()... ) )
    >
    R operator()( As&&... as ) const {
      return (*pf)( std::forward<As>(as)... );
    }
    
    /*
    // コンパイルエラー
    template<class... As>
    R operator()( Args... args, As&&... as ) const {
      return (*pf)( std::forward<Args>(args)..., std::forward<As>(as)... );
    }
    */
    
    // 内部のポインタを取得する
    pointer get() const { return pf; }
    
   private:
    pointer pf;
  };
  // ヘルパ関数
  template<class R, class... Args>
  inline pointer_to_function<R (Args..., ...)> ptr_fn( R (*f)(Args..., ...) ) {
    return f;
  }
  
} // namespace etude

#endif  // #ifndef ETUDE_FUNCTINAL_INCLUDED_PTR_FN_HPP_
