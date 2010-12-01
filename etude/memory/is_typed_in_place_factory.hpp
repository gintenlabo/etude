//
//  is_typed_in_place_factory:
//    与えられた型が TypedInPlaceFactory かどうかを判定するメタ関数
// 
//    それに加えて、 TypedInPlaceFactory ならば関連付けられた型を得る
//    （ TypedInPlaceFactory でないないなら型は定義されない）
//    メタ関数 
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_MEMORY_INCLUDED_IS_TYPED_IN_PLACE_FACTORY_HPP_
#define ETUDE_MEMORY_INCLUDED_IS_TYPED_IN_PLACE_FACTORY_HPP_

#include <boost/utility/typed_in_place_factory.hpp>

#include <type_traits>

namespace etude {

  using boost::typed_in_place_factory_base;
  
  // metafunction is_typed_in_place_factory
  
  // T が（CV修飾された） TypedInPlaceFactory またはその参照なら true を返す。
  // そうでなければ false を返す。
  template<class T>
  struct is_typed_in_place_factory :
    std::is_base_of<
      typed_in_place_factory_base, typename std::remove_reference<T>::type
    >
  {};
  
  
  // metafunction typed_in_place_associated
  
  // typed_in_place_factory なら、関連付けられてる型 value_type を得る
  // そうでなければ type は定義されない
  template<class T, class = void>
  struct typed_in_place_associated_ {};
  template<class InPlace>
  struct typed_in_place_associated_<InPlace,
    typename std::enable_if<is_typed_in_place_factory<InPlace>::value>::type
  >{
    typedef typename InPlace::value_type type;
  };
  // 本体。 enable_if 用のダミー引数は隠すのが正義ですよね。
  template<class T>
  struct typed_in_place_associated :
    typed_in_place_associated_<
      typename std::remove_reference<T>::type
    >
  {};

}

#endif  // #ifndef ETUDE_MEMORY_INCLUDED_IS_TYPED_IN_PLACE_FACTORY_HPP_
