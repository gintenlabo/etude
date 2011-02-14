//
//  unpack のテストです。
//    
//  Copyright (C) 2010-11  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../etude/unpack.hpp"

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

#include <type_traits>
#include <boost/assert.hpp>

template<class... Args>
inline std::size_t count_args( Args&&... ) {
  return sizeof...(Args);
}

template<class Tuple, std::size_t... Indices>
inline std::size_t count_args( etude::unpacked_tuple<Tuple, Indices...> t ) {
  (void)t;
  return count_args( etude::move<Indices>(t)... );
}

template<class Tuple>
void check( Tuple && t )
{
  typedef decltype( etude::unpack( std::declval<Tuple>() ) ) unpacked_type;
  
  STATIC_ASSERT((
    std::is_same<
      unpacked_type,
      typename etude::unpacked<Tuple&&>::type
    >::value
  ));
  
  STATIC_ASSERT((
    std::is_same<
      Tuple&&,
      typename unpacked_type::tuple_type
    >::value
  ));
  
  BOOST_ASSERT((
    count_args( etude::unpack(t) ) == etude::tuple_size<Tuple>::value
  ));
}

#include <tuple>
#include <utility>
#include <functional>

int main()
{
  check( std::make_tuple() );
  
  int i = 0;
  check( std::make_tuple( 1, "hoge", std::ref(i) ) );
  
  auto const t = std::make_tuple( 1, "hoge", std::ref(i) );
  check( t );
  
  check( std::make_pair( i, std::cref(i) ) );
}
