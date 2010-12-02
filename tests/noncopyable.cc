//
//  etude/noncopyable.hpp に対するテスト
//    というより仕様
//
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#include "../etude/noncopyable.hpp"

#include <type_traits>

int main()
{
  // gcc 4.5.0 にはこれらの関数は存在しない
  /*
  static_assert( !std::has_copy_constructor<etude::noncopyable>::value,
    "etude::noncopyable shall be non-copy-constructible." );
  static_assert( !std::has_copy_assign<etude::noncopyable>::value,
    "etude::noncopyable shall be non-copy-assignable." );
  
  static_assert( !std::has_move_constructor<etude::noncopyable>::value,
    "etude::noncopyable shall be non-move-constructible." );
  static_assert( !std::has_move_assign<etude::noncopyable>::value,
    "etude::noncopyable shall be non-move-assignable." );
  */
  
  static_assert( std::is_empty<etude::noncopyable>::value,
    "etude::noncopyable shall be empty." );
  
  static_assert( std::has_trivial_default_constructor<etude::noncopyable>::value,
    "etude::noncopyable shall be trivially default-constructible." );
  static_assert( std::has_trivial_destructor<etude::noncopyable>::value,
    "etude::noncopyable shall be trivially destructible." );
}
