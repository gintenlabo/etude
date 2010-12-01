//
//  noncopyable:
//    boost::noncopyable の C++0x 拡張
// 
//    boost::noncopyable を = delete; を使って書きなおした版です。
//    オリジナルの boost::noncopyable と違い、可能なかぎり trivial になっています。
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_INCLUDED_NONCOPYABLE_HPP_
#define ETUDE_INCLUDED_NONCOPYABLE_HPP_


namespace etude {
 namespace noncopyable_ { // ADL 回避
 
  struct noncopyable
  {
    noncopyable() = default;
    
    noncopyable( noncopyable const& ) = delete;
    void operator=( noncopyable const& ) = delete;
    noncopyable( noncopyable&& ) = delete;
    void operator=( noncopyable&& ) = delete;
    
  };
 
 }  // ADL 回避
 using namespace noncopyable_;
}

#endif  // #ifndef ETUDE_INCLUDED_NONCOPYABLE_HPP_
