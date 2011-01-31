//
//  storage_size :
//    指定された型をメンバとして持つ場合に必要な領域の大きさを得る
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_TYPES_INCLUDED_STORAGE_SIZE_HPP_
#define ETUDE_TYPES_INCLUDED_STORAGE_SIZE_HPP_

#include "size_of.hpp"
#include "make_struct.hpp"

namespace etude {

  // 通常は size_of と同じ
  template<class T>
  struct storage_size
    : size_of<T> {};
  
  // 参照の場合は、参照をメンバとして持つ構造体のサイズ
  template<class T>
  struct storage_size<T&>
    : size_of<typename make_struct<T&>::type> {};

} // namespace etude

#endif  // #ifndef ETUDE_TYPES_INCLUDED_STORAGE_SIZE_HPP_
