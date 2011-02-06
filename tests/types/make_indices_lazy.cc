//
//  make_indices_lazy のテストです。
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/types/make_indices_lazy.hpp"

#include <type_traits>

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

template<class IntegralConstant>
void check()
{
  typedef etude::make_indices_lazy<IntegralConstant> tested;
  
  static std::size_t const N = IntegralConstant::value;
  typedef typename etude::make_indices<N>::type indices_t;
  
  STATIC_ASSERT((
    std::is_same< indices_t, typename tested::type >::value
  ));
  STATIC_ASSERT((
    std::is_base_of< indices_t, tested >::value
  ));
}

template<class T>
void check_not_defined()
{
  typedef etude::make_indices_lazy<T> tested;
  
  STATIC_ASSERT((
    std::is_same< etude::void_type, typename tested::type >::value
  ));
  STATIC_ASSERT((
    std::is_base_of< etude::void_type, tested >::value
  ));
}

#include "../../etude/types/null_constant.hpp"
#include "../../etude/types/size_constant.hpp"

struct hoge {
  static std::size_t const value = 3;
};
struct fuga {};

int main()
{
  check< std::integral_constant<int, 2> >();
  check< etude::size_constant<0> >();
  check< hoge >();
  
  check_not_defined< int >();
  check_not_defined< etude::null_constant<std::size_t> >();
  check_not_defined< fuga >();
}
