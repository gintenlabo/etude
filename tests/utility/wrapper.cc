//
//  wrapper のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/utility/wrapper.hpp"

#include <boost/test/minimal.hpp>
#include <type_traits>

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

// 俺々クラス
struct X
{
  int i;
  
  X      * operator->()       { return this; }
  X const* operator->() const { return this; }
  
};

int test_main( int, char** )
{
  // scalar
  {
    etude::wrapper<int> i;
    BOOST_CHECK( i == int() );
    
    auto i_ = unwrap( std::move(i) );
    STATIC_ASSERT((
      std::is_same<decltype(i_), int>::value
    ));
    
    etude::wrapper<int> const j( 42 );
    STATIC_ASSERT((
      std::is_same<decltype( unwrap( std::move(j) ) ), int const&>::value
    ));
    
    X x = {1};
    etude::wrapper<X*> p( &x );
    BOOST_CHECK( p == &x );
    BOOST_CHECK( p->i == 1 );
  }
  
  // arrays
  {
    etude::wrapper<int [5]> a = {{ 1, 2, 3, 4, 5 }};
    BOOST_CHECK( a[1] == 2 );
    BOOST_CHECK( *( a + 2 ) == 3 );
  }
  
  // class, references
  {
    etude::wrapper<X> x( {0} );
    BOOST_CHECK( x->i == 0 );
    
    etude::wrapper<X&> r(x);
    etude::wrapper<X&&> rr( std::move( unwrap(r) ) );
    BOOST_CHECK( rr->i == 0 );
  }
  
  return 0;
}
