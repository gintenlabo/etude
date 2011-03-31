//
//  noncopyable:
//    copy は出来ないが move は出来るクラス
// 
//    boost::noncopyable の C++0x 拡張ですが、 move は可能になっています。
//    また、テンプレート引数により base class chaining を行うこともできます。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_INCLUDED_NONCOPYABLE_HPP_
#define ETUDE_INCLUDED_NONCOPYABLE_HPP_

#include <type_traits>
#include <utility>

#include "types/is_constructible.hpp"
#include "types/make_derivable.hpp"

namespace etude {
 namespace noncopyable_ { // ADL 回避
 
  template<class Base = void>
  class noncopyable
    : public etude::make_derivable<Base>::type
  {
    typedef typename etude::make_derivable<Base>::type base_;
    
   public:
    noncopyable() = default;
    
    noncopyable( noncopyable const& )    = delete;
    void operator=( noncopyable const& ) = delete;
    
    noncopyable( noncopyable&& )            = default;
    // noncopyable& operator=( noncopyable&& ) = default; // gcc 4.5.0 だと無理
    noncopyable& operator=( noncopyable && rhs ) {
      Base::operator=( static_cast<Base&&>(rhs) );
      return *this;
    }
    
    
    // Base の構築
    
    // Base 自体から構築。ただし explicit
    template< class T = Base,
      class = typename std::enable_if<
        etude::is_constructible<Base, T&&>::value
      >::type
    >
    explicit noncopyable( Base && x )
      : base_( std::forward<Base>(x) ) {}
    
    // その他の構築も全て explicit
    template< class... Args,
      class = typename std::enable_if<
        etude::is_constructible<Base, Args...>::value
      >::type
    >
    explicit noncopyable( Args&&... args )
      : base_( std::forward<Args>(args)... ) {}
    
  };
 
  template<>
  struct noncopyable<void>
  {
    noncopyable() = default;
    
    noncopyable( noncopyable const& )    = delete;
    void operator=( noncopyable const& ) = delete;
    
    noncopyable( noncopyable&& )            = default;
    // noncopyable& operator=( noncopyable&& ) = default; // gcc 4.5.0 だと無理
    noncopyable& operator=( noncopyable && ) { return *this; }
    
  };
 
 }
 using namespace noncopyable_;
}

#endif  // #ifndef ETUDE_INCLUDED_NONCOPYABLE_HPP_
