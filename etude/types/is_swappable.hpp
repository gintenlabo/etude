//
//  is_swappable :
//    swap 呼び出し出来れば true になる関数
//    
//    etude::is_assignable<T, U> は、
//    std::is_assignable<T&, U> と同じです。
//
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_TYPES_INCLUDED_IS_SWAPPABLE_HPP_
#define ETUDE_TYPES_INCLUDED_IS_SWAPPABLE_HPP_

#include <type_traits>
#include <utility>
#include "identity.hpp"
#include "bool_constant.hpp"

namespace etude {

  // ヘルパ名前空間
  namespace is_swappable_impl_
  {
    using std::swap;
  
    template< class T, class U,
      class = decltype( swap( std::declval<T>(), std::declval<U>() ) )
    >
    inline std::true_type test_( int )
      // noexcept( noexcept( swap( std::declval<T>(), std::declval<U>() ) ) )
    {
      return {};
    }
  
    template< class T, class U>
    inline std::false_type test_( ... )
      // noexcept( false )
    {
      return {};
    }
  
  }
  
  // 簡易版。対称性はチェックしない
  template<class T, class U>
  struct is_swappable_
    : etude::identity<decltype( is_swappable_impl_::test_<T, U>(0) )>::type
  {
  };

  // 通常使う版。 参照でない場合は参照にする
  template<class T>
  struct is_swappable
    : is_swappable_<T&, T&>::type {};

  // 完全版。参照でない場合は参照にする必要あり
  template<class T, class U = T>
  struct is_value_swappable :
    etude::bool_constant<
      is_swappable_<T, U>::value && is_swappable_<U, T>::value
    >::type
  {};

} // namespace etude

#endif  // #ifndef ETUDE_TYPES_INCLUDED_IS_SWAPPABLE_HPP_
