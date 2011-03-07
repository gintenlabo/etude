//
//  immovable:
//    boost::noncopyable の C++0x 拡張
// 
//    boost::noncopyable を = delete; を使って書きなおした版です。
//    copy も move も出来ないという意図を強調するため、名前を変更しています。
//    オリジナルの boost::noncopyable との違いは、可能なかぎり trivial になっている点と、
//    テンプレート引数により base class chaining が可能になっている点です。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_INCLUDED_IMMOVABLE_HPP_
#define ETUDE_INCLUDED_IMMOVABLE_HPP_

#include <type_traits>
#include <utility>

namespace etude {
 namespace immovable_ { // ADL 回避
 
  template<class Base = void>
  struct immovable
    : Base
  {
    immovable() = default;
    
    immovable( immovable const& )      = delete;
    void operator=( immovable const& ) = delete;
    
    // なくてもよい
    immovable( immovable&& )           = delete;
    void operator=( immovable&& )      = delete;
    
    
    // Base の構築
    
    // Base 自体から構築。ただし explicit
    template< class T = Base,
      class = typename std::enable_if<
        std::is_constructible<Base, T&&>::value
      >::type
    >
    explicit immovable( Base && x )
      : Base( std::forward<Base>(x) ) {}
    
    // その他の構築も全て explicit
    template< class... Args,
      class = typename std::enable_if<
        std::is_constructible<Base, Args...>::value
      >::type
    >
    explicit immovable( Args&&... args )
      : Base( std::forward<Args>(args)... ) {}
    
  };
  
  template<>
  struct immovable<void>
  {
    immovable() = default;
    
    immovable( immovable const& )      = delete;
    void operator=( immovable const& ) = delete;
    
    immovable( immovable&& )      = delete;
    void operator=( immovable&& ) = delete;
    
  };
 
 }
 using namespace immovable_;
}

#endif  // #ifndef ETUDE_INCLUDED_IMMOVABLE_HPP_
