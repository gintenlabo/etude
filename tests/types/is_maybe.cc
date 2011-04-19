//
//  is_maybe のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/types/is_maybe.hpp"

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

struct Maybe
{
  explicit operator bool() /*non-const*/ { return true; }
  int operator*() /*non-const*/ { return 42; }
};

struct NonMaybeA
{
  explicit operator bool() const { return true; }
};
struct NonMaybeB
{
  int operator*() const { return 42; }
};

int main()
{
  // 組み込み型
  STATIC_ASSERT(( !etude::is_maybe<void>::value ));
  STATIC_ASSERT(( !etude::is_maybe<int>::value ));
  STATIC_ASSERT(( !etude::is_maybe<void*>::value ));
  STATIC_ASSERT((  etude::is_maybe<int*>::value ));
  STATIC_ASSERT((  etude::is_maybe<void(*)()>::value ));
  STATIC_ASSERT((  etude::is_maybe<void(&)()>::value ));
  STATIC_ASSERT(( !etude::is_maybe<int Maybe::*>::value ));
  
  // ユーザ定義型
  STATIC_ASSERT((  etude::is_maybe<Maybe>::value ));
  STATIC_ASSERT((  etude::is_maybe<Maybe&>::value ));
  STATIC_ASSERT((  etude::is_maybe<Maybe&&>::value ));
  STATIC_ASSERT(( !etude::is_maybe<Maybe const>::value ));
  STATIC_ASSERT(( !etude::is_maybe<Maybe const&>::value ));
  STATIC_ASSERT(( !etude::is_maybe<Maybe const&&>::value ));
  STATIC_ASSERT(( !etude::is_maybe<NonMaybeA>::value ));
  STATIC_ASSERT(( !etude::is_maybe<NonMaybeA const&>::value ));
  STATIC_ASSERT(( !etude::is_maybe<NonMaybeB>::value ));
  STATIC_ASSERT(( !etude::is_maybe<NonMaybeB const&>::value ));
}
