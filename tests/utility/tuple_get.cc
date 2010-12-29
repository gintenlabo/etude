//
//  tuple_get のテストです。
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/utility/tuple_get.hpp"

#include "../../etude/types/tuple_size.hpp"
#include <boost/utility/addressof.hpp>
#include <boost/assert.hpp>
#include <type_traits>
#include <utility>
#include <tuple>

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

template<std::size_t I, class Tuple>
inline void check( Tuple && t )
{
  typedef typename std::remove_reference<Tuple>::type tuple_type;
  
  // 型チェック
  STATIC_ASSERT((
    std::is_same<
      decltype( etude::tuple_get<I>( std::forward<Tuple>(t) ) ),
      typename etude::tuple_element<I, Tuple>::type &&
    >::value
  ));
  STATIC_ASSERT((
    std::is_same<
      decltype( etude::tuple_move<I>(t) ),
      typename etude::tuple_element<I, tuple_type>::type &&
    >::value
  ));
  STATIC_ASSERT((
    std::is_same<
      decltype( etude::tuple_forward<Tuple, I>(t) ),
      typename etude::tuple_element<I, Tuple>::type &&
    >::value
  ));
  
  // アドレスチェック
  using std::get;
  auto && o = get<I>(t);
  
  auto && x = etude::tuple_get<I>(t);
  BOOST_ASSERT( boost::addressof(o) == boost::addressof(x) );
  
  auto && y = etude::tuple_move<I>(t);
  BOOST_ASSERT( boost::addressof(o) == boost::addressof(y) );
  
  auto && z = etude::tuple_forward<Tuple, I>(t);
  BOOST_ASSERT( boost::addressof(o) == boost::addressof(z) );
}

template<std::size_t I, class Tuple,
  class = typename std::enable_if<( I >= etude::tuple_size<Tuple>::value )>::type
>
inline void test_( Tuple &&, std::integral_constant<std::size_t, I>, ... ) {}

template<std::size_t I, class Tuple,
  class = typename std::enable_if<( I < etude::tuple_size<Tuple>::value )>::type
>
inline void test_( Tuple && t, std::integral_constant<std::size_t, I> ) {
  check<I>( std::forward<Tuple>(t) );
  return test_( std::forward<Tuple>(t), std::integral_constant<std::size_t, I+1>() );
}

template<class Tuple>
inline void test( Tuple && t ) {
  return test_( std::forward<Tuple>(t), std::integral_constant<std::size_t, 0>() );
}

#include <array>

class klass {};

template<class T>
inline T const& to_const( T const& x ) {
  return x;
}

int main()
{
  klass k;
  std::tuple<int, char const*, klass&> t( 0, "hoge", k );
  
  test( t );
  test( to_const(t) );
  test( std::move(t) );
  
  std::array<int, 2> a;
  test( a );
  test( to_const(a) );
  test( std::move(a) );
  
  std::pair<double const, klass &&> p( 0, std::move(k) );
  test( p );
  test( to_const(p) );
  test( std::move(p) );
}
