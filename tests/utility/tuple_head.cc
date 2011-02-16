//
//  tuple_head のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/utility/tuple_head.hpp"

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
  
  // 型チェック
  STATIC_ASSERT((
    std::is_same<
      decltype( etude::tuple_head( std::forward<Tuple>(t) ) ),
      typename etude::tuple_element<0, Tuple>::type &&
    >::value
  ));
  
  // アドレスチェック
  auto && o = etude::tuple_get<0>(t);
  
  auto && x = etude::tuple_head(t);
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
  
  std::tuple<int, char const*, klass&, klass const&> t1( 0, "hoge", k, k );
  check( t1 );
  check( to_const(t1) );
  check( std::move(t1) );
  
  std::tuple<klass&, int> t2( k, 0 );
  check( t2 );
  check( to_const(t2) );
  check( std::move(t2) );
  
  std::tuple<klass const&, char*> t3( k, static_cast<char*>(0) );
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
  
  std::pair<double const, char> p( 0, 'a' );
  check( p );
  check( to_const(p) );
  check( std::move(p) );
}
