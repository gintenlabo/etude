//
//  storage_of :
//    与えられた型全てを格納できるストレージクラス
//    
//    storage_of<Ts...>::type は、 Ts... で指定される全ての型を
//    格納することのできる POD 型に定義されます。
//    
//    これは
//    std::aligned_storage<storage_of<Ts...>::size, storage_of<Ts...>::align>::type
//    と同じ型になります。
//    storage_of<Ts...>::size および storage_of<Ts...>::align は、それぞれ
//    storage_size<Ts>::value... および storage_align<Ts>::value... のうち最大の値です。
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_TYPES_INCLUDED_STORAGE_OF_HPP_
#define ETUDE_TYPES_INCLUDED_STORAGE_OF_HPP_

#include <type_traits>
#include "storage_size.hpp"
#include "storage_align.hpp"

namespace etude {

  template<class... Types>
  struct storage_of_;
  
  template<>
  struct storage_of_<>
  {
    static const std::size_t size  = 1;
    static const std::size_t align = 1;
    
    static const bool is_empty = true;
    
  };
  
  template<class T, class... Types>
  class storage_of_<T, Types...>
  {
    typedef storage_of_<Types...> tail;
    static const std::size_t s1 = storage_size<T>::value,  s2 = tail::size;
    static const std::size_t a1 = storage_align<T>::value, a2 = tail::align;
    
    static_assert( s1 != 0 && a1 != 0, "T must be object or reference." );
    
   public:
    static const std::size_t size  = (s1>s2) ? s1 : s2;
    static const std::size_t align = (a1>a2) ? a1 : a2;
    
    static const bool is_empty = tail::is_empty && std::is_empty<T>::value;
    
  };
  
  // 与えられた型全てを格納できるストレージクラス
  template<class... Ts>
  struct storage_of
  {
    static const std::size_t size  = storage_of_<Ts...>::size;
    static const std::size_t align = storage_of_<Ts...>::align;
    
    static const bool is_empty = storage_of_<Ts...>::is_empty;
    
    typedef typename std::aligned_storage<size, align>::type type;
    
  };

} // namespace etude

#endif  // #ifndef ETUDE_TYPES_INCLUDED_STORAGE_OF_HPP_
