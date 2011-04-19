//
//  partially_ordered:
//    equality_comparable かつ weakly_ordered
// 
//    boost::partially_ordered と違い、速度の劣化がありません。
//  
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_OPERATORS_INCLUDED_PARTIALLY_ORDERED_HPP_
#define ETUDE_OPERATORS_INCLUDED_PARTIALLY_ORDERED_HPP_

#include "../utility/empty_base.hpp"

#include "equality_comparable.hpp"
#include "weakly_ordered.hpp"

namespace etude {
 namespace partially_ordered_ {
 
  template< class T, class Base = etude::empty_base<T> >
  struct partially_ordered1
    : weakly_ordered1< T, equality_comparable1<T, Base> > {};
 
  template< class T, class U, class Base = etude::empty_base<T> >
  struct partially_ordered2
    : weakly_ordered2< T, U, equality_comparable2<T, U, Base> > {};
  
  
  // Boost.Operators のようなメタプログラミングは行わない
  template< class T, class U = T, class Base = etude::empty_base<T> >
  class partially_ordered;
  
  template<class T, class Base>
  struct partially_ordered<T, T, Base>
    : partially_ordered1<T, Base> {};
  
  template<class T, class U, class Base>
  struct partially_ordered
    : partially_ordered2<T, U, Base> {};
  
 
 } // namespace partially_ordered_
 using namespace partially_ordered_;
 
} // namespace etude

#endif  // #ifndef ETUDE_OPERATORS_INCLUDED_PARTIALLY_ORDERED_HPP_
