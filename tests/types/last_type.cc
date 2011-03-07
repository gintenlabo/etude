//
//  last_type のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/types/last_type.hpp"

#include <type_traits>

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

// チェック用関数
template< class Checked, class Expected >
void check()
{
  STATIC_ASSERT(( std::is_same<typename Checked::type, Expected>::value ));
}

#include "../../etude/types/get_type_or.hpp"

template< class Checked >
void check_not_defined()
{
  class X {};
  
  STATIC_ASSERT((
    std::is_same< X,
      typename etude::get_type_or<Checked, X>::type
    >::value
  ));
}

int main()
{
  check_not_defined< etude::last_type<> >();
  check< etude::last_type<int>, int >();
  check< etude::last_type<double, char>, char >();
  check< etude::last_type<char, int, double>, double >();
}
