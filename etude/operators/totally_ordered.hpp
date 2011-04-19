//
//  totally_ordered:
//    equality_comparable かつ strictly_ordered
// 
//    SFINAE する以外は boost::totally_ordered と同じです。
//  
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_OPERATORS_INCLUDED_TOTALLY_ORDERED_HPP_
#define ETUDE_OPERATORS_INCLUDED_TOTALLY_ORDERED_HPP_

#include "../utility/empty_base.hpp"

#include "equality_comparable.hpp"
#include "strictly_ordered.hpp"

namespace etude {
 namespace totally_ordered_ {
 
  template< class T, class Base = etude::empty_base<T> >
  struct totally_ordered1
    : strictly_ordered1< T, equality_comparable1<T, Base> > {};
 
  template< class T, class U, class Base = etude::empty_base<T> >
  struct totally_ordered2
    : strictly_ordered2< T, U, equality_comparable2<T, U, Base> > {};
  
  
  // Boost.Operators のようなメタプログラミングは行わない
  template< class T, class U = T, class Base = etude::empty_base<T> >
  class totally_ordered;
  
  template<class T, class Base>
  struct totally_ordered<T, T, Base>
    : totally_ordered1<T, Base> {};
  
  template<class T, class U, class Base>
  struct totally_ordered
    : totally_ordered2<T, U, Base> {};
  
 
 } // namespace totally_ordered_
 using namespace totally_ordered_;
 
} // namespace etude

#endif  // #ifndef ETUDE_OPERATORS_INCLUDED_TOTALLY_ORDERED_HPP_
