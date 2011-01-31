//
//  storage_align :
//    指定された型をメンバとして持つ場合に必要な領域のアライメントを得る
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_TYPES_INCLUDED_STORAGE_ALIGN_HPP_
#define ETUDE_TYPES_INCLUDED_STORAGE_ALIGN_HPP_

#include "align_of.hpp"
#include "make_struct.hpp"

namespace etude {

  // 通常は align_of と同じ
  template<class T>
  struct storage_align
    : align_of<T> {};
  
  // 参照の場合は、参照をメンバとして持つ構造体のアライメント
  template<class T>
  struct storage_align<T&>
    : align_of<typename make_struct<T&>::type> {};

} // namespace etude

#endif  // #ifndef ETUDE_TYPES_INCLUDED_STORAGE_ALIGN_HPP_
