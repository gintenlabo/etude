//
//  is_decay_copyable のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/types/is_decay_copyable.hpp"

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

#include "../../etude/noncopyable.hpp"
#include "../../etude/immovable.hpp"

int main()
{
  // (cv) void は decay_copyable ではない
  STATIC_ASSERT(( !etude::is_decay_copyable<void>::value ));
  STATIC_ASSERT(( !etude::is_decay_copyable<void const>::value ));
  STATIC_ASSERT(( !etude::is_decay_copyable<void volatile>::value ));
  STATIC_ASSERT(( !etude::is_decay_copyable<void const volatile>::value ));
  
  // それ以外の組み込み型は decay_copyable
  STATIC_ASSERT(( etude::is_decay_copyable<int>::value ));
  STATIC_ASSERT(( etude::is_decay_copyable<int const>::value ));
  STATIC_ASSERT(( etude::is_decay_copyable<int const volatile>::value ));
  STATIC_ASSERT(( etude::is_decay_copyable<int&>::value ));
  STATIC_ASSERT(( etude::is_decay_copyable<void*>::value ));
  STATIC_ASSERT(( etude::is_decay_copyable<void**>::value ));
  STATIC_ASSERT(( etude::is_decay_copyable<void* const*>::value ));
  
  // ユーザ定義型
  struct X {};
  
  STATIC_ASSERT((  etude::is_decay_copyable<X>::value ));
  STATIC_ASSERT((  etude::is_decay_copyable<X&>::value ));
  STATIC_ASSERT((  etude::is_decay_copyable<X&&>::value ));
  STATIC_ASSERT((  etude::is_decay_copyable<X const>::value ));
  STATIC_ASSERT((  etude::is_decay_copyable<X const&>::value ));
  STATIC_ASSERT(( !etude::is_decay_copyable<X volatile>::value ));
  STATIC_ASSERT(( !etude::is_decay_copyable<X volatile&>::value ));
  
  // non-copyable but movable
  STATIC_ASSERT((  etude::is_decay_copyable<etude::noncopyable<>>::value ));
  STATIC_ASSERT(( !etude::is_decay_copyable<etude::noncopyable<>&>::value ));
  STATIC_ASSERT((  etude::is_decay_copyable<etude::noncopyable<>&&>::value ));
  STATIC_ASSERT(( !etude::is_decay_copyable<etude::noncopyable<> const>::value ));
  STATIC_ASSERT(( !etude::is_decay_copyable<etude::noncopyable<> const&>::value ));
  STATIC_ASSERT(( !etude::is_decay_copyable<etude::noncopyable<> volatile>::value ));
  STATIC_ASSERT(( !etude::is_decay_copyable<etude::noncopyable<> volatile&>::value ));
  
  // immovable
  STATIC_ASSERT(( !etude::is_decay_copyable<etude::immovable<>>::value ));
  STATIC_ASSERT(( !etude::is_decay_copyable<etude::immovable<>&>::value ));
  STATIC_ASSERT(( !etude::is_decay_copyable<etude::immovable<>&&>::value ));
  STATIC_ASSERT(( !etude::is_decay_copyable<etude::immovable<> const>::value ));
  STATIC_ASSERT(( !etude::is_decay_copyable<etude::immovable<> const&>::value ));
  STATIC_ASSERT(( !etude::is_decay_copyable<etude::immovable<> volatile>::value ));
  STATIC_ASSERT(( !etude::is_decay_copyable<etude::immovable<> volatile&>::value ));
}
