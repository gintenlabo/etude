//
//  tuple_last のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/utility/tuple_last.hpp"

#include "../../etude/types/tuple_size.hpp"
#include <boost/utility/addressof.hpp>
#include <boost/assert.hpp>
#include <type_traits>
#include <utility>
#include <tuple>

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

template<class Tuple>
inline void check( Tuple && t )
{
  typedef typename std::remove_reference<Tuple>::type tuple_type;
  static const std::size_t N = etude::tuple_size<Tuple>::value - 1;
  
  // 型チェック
  STATIC_ASSERT((
    std::is_same<
      decltype( etude::tuple_last( std::forward<Tuple>(t) ) ),
      typename etude::tuple_element<N, Tuple>::type &&
    >::value
  ));
  
  // アドレスチェック
  auto && o = etude::tuple_get<N>(t);
  
  auto && x = etude::tuple_last(t);
  BOOST_ASSERT( boost::addressof(o) == boost::addressof(x) );
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
  
  std::tuple<char const*, klass&, klass const&, int> t1( "hoge", k, k, 0 );
  check( t1 );
  check( to_const(t1) );
  check( std::move(t1) );
  
  std::tuple<int, klass&> t2( 0, k );
  check( t2 );
  check( to_const(t2) );
  check( std::move(t2) );
  
  std::tuple<char*, klass const&> t3( static_cast<char*>(0), k );
  check( t3 );
  check( to_const(t3) );
  check( std::move(t3) );
  
  std::tuple<klass&&> t4( std::move(k) );
  check( t4 );
  check( to_const(t4) );
  check( std::move(t4) );
  
  std::array<int, 2> a;
  check( a );
  check( to_const(a) );
  check( std::move(a) );
  
  std::pair<double const, char const> p( 0, 'a' );
  check( p );
  check( to_const(p) );
  check( std::move(p) );
}
