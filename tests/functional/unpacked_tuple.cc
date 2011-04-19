//
//  unpacked_tuple のテストです。
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/functional/unpacked_tuple.hpp"

#include "../../etude/types/tuple_size.hpp"
#include <boost/utility/addressof.hpp>
#include <boost/assert.hpp>
#include <type_traits>
#include <utility>
#include <tuple>

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

template<std::size_t I, class Tuple>
inline void check()
{
  typedef typename Tuple::tuple_type tuple_type;
  
  // 型チェック
  STATIC_ASSERT((
    std::is_same<
      decltype( etude::get<I>( std::declval<Tuple&>() ) ),
      typename etude::tuple_element<I, tuple_type&>::type
    >::value
  ));
  STATIC_ASSERT((
    std::is_same<
      decltype( etude::get<I>( std::declval<Tuple const&>() ) ),
      typename etude::tuple_element<I, tuple_type const&>::type
    >::value
  ));
  STATIC_ASSERT((
    std::is_same<
      decltype( etude::get<I>( std::declval<Tuple>() ) ),
      typename etude::tuple_element<I, tuple_type&&>::type
    >::value
  ));
  
  STATIC_ASSERT((
    std::is_same<
      decltype( etude::move<I>( std::declval<Tuple&>() ) ),
      typename etude::tuple_element<I, tuple_type&&>::type
    >::value
  ));
  STATIC_ASSERT((
    std::is_same<
      decltype( etude::move<I>( std::declval<Tuple>() ) ),
      typename etude::tuple_element<I, tuple_type&&>::type
    >::value
  ));
}

template<std::size_t I, class Tuple,
  class = typename std::enable_if<( I >= etude::tuple_size<typename Tuple::tuple_type>::value )>::type
>
inline void test_( Tuple, std::integral_constant<std::size_t, I>, ... ) {}

template<std::size_t I, class Tuple,
  class = typename std::enable_if<( I < etude::tuple_size<typename Tuple::tuple_type>::value )>::type
>
inline void test_( Tuple t, std::integral_constant<std::size_t, I> ) {
  check<I, Tuple>();
  return test_( std::move(t), std::integral_constant<std::size_t, I+1>() );
}

template<class Tuple>
inline void test( Tuple t ) {
  return test_( std::move(t), std::integral_constant<std::size_t, 0>() );
}

#include <array>

class klass {};


int main()
{
  typedef std::tuple<int, char const*, klass&, klass const&> tuple_type;
  
  klass k;
  etude::unpacked_tuple<tuple_type, 0, 1, 2, 3> t1( 0, "hoge", k, k );
  etude::unpacked_tuple<tuple_type&, 0, 1, 2, 3> t2 = t1.get_tuple();
  etude::unpacked_tuple<tuple_type const&, 0, 1, 2, 3> t3 = t1.get_tuple();
  etude::unpacked_tuple<tuple_type &&, 0, 1, 2, 3> t4 = t1.move_tuple();
  
  test( std::move(t1) );
  test( std::move(t2) );
  test( std::move(t3) );
  test( std::move(t4) );
  
  typedef std::array<int, 2> array_type;
  etude::unpacked_tuple<array_type, 0, 1> a1;
  etude::unpacked_tuple<array_type&, 0, 1> a2 = a1.get_tuple();
  etude::unpacked_tuple<array_type const&, 0, 1> a3 = a1.get_tuple();
  etude::unpacked_tuple<array_type &&, 0, 1> a4 = a1.move_tuple();
  
  test( std::move(a1) );
  test( std::move(a2) );
  test( std::move(a3) );
  test( std::move(a4) );
  
}
