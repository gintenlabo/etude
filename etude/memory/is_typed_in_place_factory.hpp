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
#ifndef ETUDE_MEMORY_INCLUDED_IS_TYPED_IN_PLACE_FACTORY_
#define ETUDE_MEMORY_INCLUDED_IS_TYPED_IN_PLACE_FACTORY_

#include <boost/utility/typed_in_place_factory.hpp>

#include <type_traits>

namespace etude {

  using boost::typed_in_place_factory_base;
  
  // metafunction get_in_place_binding_type
  
  // T が（CV修飾された） TypedInPlaceFactory またはその参照なら true を返す。
  // そうでなければ false を返す。
  template<class T>
  struct is_typed_in_place_factory :
    std::is_base_of<
      typed_in_place_factory_base, typename std::remove_reference<T>::type
    >
  {};
  
  
  // metafunction get_in_place_binding_type
  
  // typed_in_place_factory なら、関連付けられてる型 value_type を得る
  // そうでなければ type は定義されない
  template<class T, class = void>
  struct get_in_place_binding_type_ {};
  template<class InPlace>
  struct get_in_place_binding_type_<InPlace,
    typename std::enable_if<is_typed_in_place_factory<InPlace>::value>::type
  >{
    typedef typename InPlace::value_type type;
  };
  template<class T>
  struct get_in_place_binding_type :
    get_in_place_binding_type_<
      typename std::remove_reference<T>::type
    >
  {};

}

#endif  // #ifndef ETUDE_MEMORY_INCLUDED_IS_TYPED_IN_PLACE_FACTORY_
