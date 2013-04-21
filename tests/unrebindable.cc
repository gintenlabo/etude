//
//  etude/unrebindable.hpp に対するテスト
//    というより仕様
//
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#include "../etude/unrebindable.hpp"

#include <type_traits>
#include "../etude/types/is_constructible.hpp"
#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

// for is_xxx_constructible/assignable
#include "test_utilities.hpp"

int main()
{
  // etude::unrebindable<> に対して
  STATIC_ASSERT((  is_copy_constructible<etude::unrebindable<>>::value ));
  STATIC_ASSERT((  is_move_constructible<etude::unrebindable<>>::value ));
  
  STATIC_ASSERT(( !is_copy_assignable<etude::unrebindable<>>::value ));
  STATIC_ASSERT(( !is_move_assignable<etude::unrebindable<>>::value ));
  
  STATIC_ASSERT(( std::is_empty<etude::unrebindable<>>::value ));
  
  STATIC_ASSERT(( std::has_trivial_default_constructor<etude::unrebindable<>>::value ));
  STATIC_ASSERT(( etude::is_trivially_destructible<etude::unrebindable<>>::value ));


  // base class chaining
  struct X {};
  
  STATIC_ASSERT(( std::is_base_of<X, etude::unrebindable<X>>::value ));

  STATIC_ASSERT((  is_copy_constructible<etude::unrebindable<X>>::value ));
  STATIC_ASSERT((  is_move_constructible<etude::unrebindable<X>>::value ));
  
  STATIC_ASSERT(( !is_copy_assignable<etude::unrebindable<X>>::value ));
  STATIC_ASSERT(( !is_move_assignable<etude::unrebindable<X>>::value ));
  
  // ctor 転送
  STATIC_ASSERT(( !std::is_convertible<X&&, etude::unrebindable<X>>::value ));
  STATIC_ASSERT(( etude::is_constructible<etude::unrebindable<X>, X&&>::value ));
  STATIC_ASSERT(( !etude::is_constructible<etude::unrebindable<X>, int>::value ));


  // unrebindable wrapper
  STATIC_ASSERT((  is_copy_constructible<etude::unrebindable<int>>::value ));
  STATIC_ASSERT((  is_move_constructible<etude::unrebindable<int>>::value ));
  
  STATIC_ASSERT(( !is_copy_assignable<etude::unrebindable<int>>::value ));
  STATIC_ASSERT(( !is_move_assignable<etude::unrebindable<int>>::value ));
  
  // ctor 転送
  STATIC_ASSERT(( !std::is_convertible<int, etude::unrebindable<int>>::value ));
  STATIC_ASSERT(( etude::is_constructible<etude::unrebindable<int>, int>::value ));
  STATIC_ASSERT(( !etude::is_constructible<etude::unrebindable<int>, void*>::value ));
}
