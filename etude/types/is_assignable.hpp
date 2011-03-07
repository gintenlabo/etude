//
//  is_assignable :
//    std::is_assignable の改良版
//    
//    etude::is_assignable<T, U> は、
//    std::is_assignable<T&, U> と同じです。
//
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_TYPES_INCLUDED_IS_ASSIGNABLE_HPP_
#define ETUDE_TYPES_INCLUDED_IS_ASSIGNABLE_HPP_

#include <type_traits>
#include <utility>

namespace etude {

  // 実装クラス（ gcc4.5.0 だと std::is_assignable は未実装）
  template<class T_, class U>
  class is_assignable_
  {
    template< class T = T_,
      class = decltype( std::declval<T>() = std::declval<U>() )
    >
    static std::true_type test_( int );
    
    static std::false_type test_( ... );
    
   public:
    typedef decltype( test_(0) ) type;
    
  };

  // 実装クラスに転送
  template<class T, class U>
  struct is_assignable
    : is_assignable_<T&, U>::type {};

} // namespace etude

#endif  // #ifndef ETUDE_TYPES_INCLUDED_IS_ASSIGNABLE_HPP_
