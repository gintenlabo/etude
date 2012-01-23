//
//  unrebindable:
//    再代入できないクラス
// 
//    再代入を delete したクラスです．
//    また、テンプレート引数により base class chaining を行うこともできます。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_INCLUDED_UNREBINDABLE_HPP_
#define ETUDE_INCLUDED_UNREBINDABLE_HPP_

#include <type_traits>
#include <utility>

#include "types/is_constructible.hpp"
#include "types/make_derivable.hpp"

namespace etude {
 namespace unrebindable_ { // ADL 回避
 
  template<class Base = void>
  class unrebindable
    : public etude::make_derivable<Base>::type
  {
    typedef typename etude::make_derivable<Base>::type base_;
    
   public:
    unrebindable() = default;
    
    // operator= を明示的に禁止する関係で必要
    unrebindable( unrebindable const& ) = default;
    unrebindable( unrebindable && )     = default;
    
    // 再代入禁止
    void operator=( unrebindable const& ) = delete;
    // なくてもよい
    void operator=( unrebindable&& )      = delete;
    
    
    // Base の構築
    
    // Base 自体から構築。ただし explicit
    template< class T = Base,
      class = typename std::enable_if<
        etude::is_constructible<Base, T&&>::value
      >::type
    >
    explicit unrebindable( Base && x )
      : base_( std::forward<Base>(x) ) {}
    
    // その他の構築も全て explicit
    template< class... Args,
      class = typename std::enable_if<
        etude::is_constructible<Base, Args...>::value
      >::type
    >
    explicit unrebindable( Args&&... args )
      : base_( std::forward<Args>(args)... ) {}
    
  };
 
  template<>
  struct unrebindable<void>
  {
    unrebindable() = default;
    
    // operator= を明示的に禁止する関係で必要
    unrebindable( unrebindable const& ) = default;
    unrebindable( unrebindable && )     = default;
    
    // 再代入禁止
    void operator=( unrebindable const& ) = delete;
    // なくてもよい
    void operator=( unrebindable&& )      = delete;
    
  };
 
 }
 using namespace unrebindable_;
}

#endif  // #ifndef ETUDE_INCLUDED_UNREBINDABLE_HPP_
