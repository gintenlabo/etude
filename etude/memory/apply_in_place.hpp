//
//  apply_in_place:
//    Boost.InPlaceFactory の適用関数
// 
//    InPlaceFactory の apply を自由関数で行えるようにしたものです。
//    これにより、 move 出来る場合は保持している引数を move するようになるので、
//    安全性や表記の簡潔さを失わず、より効率的なコードを書けるようになります。
//    また、 InPlaceFactory と TypedInPlaceFactory で微妙に表記が異なるのを避けるため、
//    TypedInPlaceFactory でも、関連付けられた型が一致するなら、
//    InPlaceFactory と同様のインターフェイスで扱えるように工夫されています。
//    その際、 apply_in_place が呼び出せるかどうかを問い合わせるメタ関数も実装。
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_MEMORY_INCLUDED_APPLY_IN_PLACE_HPP_
#define ETUDE_MEMORY_INCLUDED_APPLY_IN_PLACE_HPP_

#include "is_in_place_factory.hpp"
#include "is_typed_in_place_factory.hpp"

#include <utility>
#include <type_traits>

namespace etude {

  // function template apply_typed_in_place
  // TypedInPlaceFactory の apply の自由関数版
  
  // 一般用
  template<class TypedInPlace,
    class T = typename std::remove_reference<TypedInPlace>::type::value_type
  >
  inline T* apply_typed_in_place( TypedInPlace && x, void* addr ) {
    std::forward<TypedInPlace>(x).apply( addr );
    return static_cast<T*>( addr );
  }
  
  // etude:: typed_in_place_factory 版は typed_in_place_factory.hpp で定義されている
  
  
  // function template apply_in_place
  // InPlaceFactory の template apply の自由関数版
  // 加えて、 TypedInPlaceFactory も受け入れられるように
  
  // 実装用クラス
  
  // apply_in_place<T>( x, addr ) と書けるかどうか
  template<class InPlace, class T, class = void>
  struct is_in_place_applyable_
    : is_in_place_factory<InPlace> {};
  // associated type が定義されない場合は SFINAE で上の定義が使われる
  template<class InPlace, class T>
  struct is_in_place_applyable_< InPlace, T,
    typename std::enable_if<
      std::is_same< T,
        typename typed_in_place_associated<InPlace>::type
      >::value
    >::type
  > : std::true_type {};
  // 実装用メタ関数とインターフェイスは分けるのが僕の流儀
  template<class InPlace, class T>
  struct is_in_place_applyable : is_in_place_applyable_<InPlace, T> {};
  
  // InPlaceFactory と TypedInPlaceFactory で処理を分ける
  // InPlaceFactory 用
  template<class T, class InPlace>
  inline T* apply_in_place_( InPlace && x, void* addr, ... ) {
    std::forward<InPlace>(x).template apply<T>( addr );
    return static_cast<T*>( addr );
  }
  // TypedInPlaceFactory 用
  template<class T, class TypedInPlace,
    class = typename std::enable_if<
      std::is_same< T, 
        typename std::remove_reference<TypedInPlace>::type::value_type
      >::value
    >::type>
  inline T* apply_in_place_( TypedInPlace && x, void* addr, int ) {
    return apply_typed_in_place( std::forward<TypedInPlace>(x), addr );
  }
  
  // apply_in_place 本体
  template<class T, class InPlace>
  inline T* apply_in_place( InPlace && x, void* addr )
  {
    return apply_in_place_<T>( std::forward<InPlace>(x), addr, 0 );
  }
}

#endif  // #ifndef ETUDE_MEMORY_INCLUDED_APPLY_IN_PLACE_HPP_
