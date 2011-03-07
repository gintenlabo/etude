//
//  make_types_from_indices のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/types/make_types_from_indices.hpp"

#include <type_traits>

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

template<class Indices, std::size_t... Is>
void check()
{
  typedef etude::make_types_from_indices<Indices> tested;
  typedef typename etude::types<std::integral_constant<std::size_t, Is>...> types_t;
  
  STATIC_ASSERT((
    std::is_same< types_t, typename tested::type >::value
  ));
  STATIC_ASSERT((
    std::is_base_of< types_t, tested >::value
  ));
}

#include "../../etude/types/get_type_or.hpp"
template<class T>
void check_not_defined()
{
  typedef etude::make_types_from_indices<T> tested;
  
  STATIC_ASSERT((
    std::is_same< typename etude::get_type_or<tested>::type, void >::value
  ));
}

#include "../../etude/types/identity.hpp"
#include "../../etude/types/null_constant.hpp"

int main()
{
  check< etude::indices<> >();
  check< etude::indices<1, 2>, 1, 2 >();
  
  check< etude::make_indices<3>, 0, 1, 2 >();
  check< etude::identity< etude::indices<0> >, 0 >();
  
  check_not_defined< int >();
  check_not_defined< etude::null_constant<std::size_t> >();
  check_not_defined< etude::identity< etude::make_indices<3> > >();
}
