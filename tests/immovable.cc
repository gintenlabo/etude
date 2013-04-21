//
//  etude/immovable.hpp に対するテスト
//    というより仕様
//
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#include "../etude/immovable.hpp"

#include <type_traits>
#include "../etude/types/is_constructible.hpp"
#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

// for is_xxx_constructible/assignable
#include "test_utilities.hpp"

int main()
{
  // etude::immovable<> に対して
  STATIC_ASSERT(( !is_copy_constructible<etude::immovable<>>::value ));
  STATIC_ASSERT(( !is_move_constructible<etude::immovable<>>::value ));
  
  STATIC_ASSERT(( !is_copy_assignable<etude::immovable<>>::value ));
  STATIC_ASSERT(( !is_move_assignable<etude::immovable<>>::value ));
  
  STATIC_ASSERT(( std::is_empty<etude::immovable<>>::value ));
  
  STATIC_ASSERT(( std::has_trivial_default_constructor<etude::immovable<>>::value ));
  STATIC_ASSERT(( etude::is_trivially_destructible<etude::immovable<>>::value ));


  // base class chaining
  struct X {};
  
  STATIC_ASSERT(( std::is_base_of<X, etude::immovable<X>>::value ));

  STATIC_ASSERT(( !is_copy_constructible<etude::immovable<X>>::value ));
  STATIC_ASSERT(( !is_move_constructible<etude::immovable<X>>::value ));
  
  STATIC_ASSERT(( !is_copy_assignable<etude::immovable<X>>::value ));
  STATIC_ASSERT(( !is_move_assignable<etude::immovable<X>>::value ));
  
  // ctor 転送
  STATIC_ASSERT(( !std::is_convertible<X&&, etude::immovable<X>>::value ));
  STATIC_ASSERT(( etude::is_constructible<etude::immovable<X>, X&&>::value ));
  STATIC_ASSERT(( !etude::is_constructible<etude::immovable<X>, int>::value ));


  // immovable wrapper
  STATIC_ASSERT(( !is_copy_constructible<etude::immovable<int>>::value ));
  STATIC_ASSERT(( !is_move_constructible<etude::immovable<int>>::value ));
  
  STATIC_ASSERT(( !is_copy_assignable<etude::immovable<int>>::value ));
  STATIC_ASSERT(( !is_move_assignable<etude::immovable<int>>::value ));
  
  // ctor 転送
  STATIC_ASSERT(( !std::is_convertible<int, etude::immovable<int>>::value ));
  STATIC_ASSERT(( etude::is_constructible<etude::immovable<int>, int>::value ));
  STATIC_ASSERT(( !etude::is_constructible<etude::immovable<int>, void*>::value ));
}
