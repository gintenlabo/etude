//
//  is_strict_explicitly_convertible のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/types/is_strict_explicitly_convertible.hpp"

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

#include <type_traits>

template<class From, class To, bool expected>
void check()
{
  bool const expected_ = std::is_constructible<To, From>::value &&
    !std::is_convertible<From, To>::value;
  
  STATIC_ASSERT(( expected_ == expected ));
  
  typedef std::integral_constant<bool, expected> expected_type;
  
  STATIC_ASSERT((
    std::is_same<
      expected_type,
      typename etude::is_strict_explicitly_convertible<From, To>::type
    >::value
  ));
  
  STATIC_ASSERT((
    std::is_base_of<
      expected_type,
      etude::is_strict_explicitly_convertible<From, To>
    >::value
  ));
}

struct X
{
  explicit X( int ) {}
  X( void* ) {}
};

struct Y {};

int main()
{
  check<int, int, false>();
  check<int, void*, false>();
  
  check<int, X, true>();
  check<void*, X, false>();
  check<Y, X, false>();
  
  check<char, X, true>();
  check<int*, X, false>();
}
