//
//  make_indices_from_types のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/types/make_indices_from_types.hpp"

#include <type_traits>

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

template<class Types, std::size_t... Indices>
void check()
{
  typedef etude::make_indices_from_types<Types> tested;
  typedef etude::indices<Indices...> indices_t;
  
  STATIC_ASSERT((
    std::is_same< indices_t, typename tested::type >::value
  ));
  STATIC_ASSERT((
    std::is_base_of< indices_t, tested >::value
  ));
}

#include "../../etude/types/get_type_or.hpp"
template<class T>
void check_not_defined()
{
  typedef etude::make_indices_from_types<T> tested;
  
  STATIC_ASSERT((
    std::is_same< typename etude::get_type_or<tested>::type, void >::value
  ));
}

#include "../../etude/types/make_types_from_indices.hpp"
#include "../../etude/types/size_constant.hpp"
#include "../../etude/types/identity.hpp"
#include "../../etude/types/null_constant.hpp"

struct X {
  static const int value = 0;
};

int main()
{
  check< etude::types<> >();
  check< etude::types<etude::size_constant<1>, etude::size_constant<2>>, 1, 2 >();
  
  check< etude::make_types_from_indices< etude::indices<0, 1, 2> >, 0, 1, 2 >();
  check< etude::identity< etude::types<X> >, 0 >();
  
  check_not_defined< int >();
  check_not_defined< etude::types< etude::null_constant<std::size_t> > >();
  check_not_defined<
    etude::identity<
      etude::make_types_from_indices< etude::indices<0, 1, 2> >
    >
  >
  ();
}
