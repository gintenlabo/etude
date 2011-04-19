//
//  equality_comparable のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/operators/equality_comparable.hpp"

#include <boost/test/minimal.hpp>
#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

struct X
  : private etude::equality_comparable<
      X, int,
      etude::equality_comparable<X>
    >
{
  int i;
  explicit X( int i_ ) : i(i_) {}
  
  // operator==
  friend bool operator==( X const& lhs, X const& rhs ) {
    return lhs.i == rhs.i;
  }
  friend bool operator==( X const& lhs, int rhs ) {
    return lhs.i == rhs;
  }
  
};

int test_main( int, char** )
{
  X x(1), y(1), z(2);
  
  BOOST_CHECK(    x == y   );
  BOOST_CHECK( !( x != y ) );
  BOOST_CHECK(    x != z   );
  BOOST_CHECK( !( x == z ) );
  
  BOOST_CHECK(    x == 1   );
  BOOST_CHECK( !( x != 1 ) );
  BOOST_CHECK(    x != 2   );
  BOOST_CHECK( !( x == 2 ) );
  BOOST_CHECK(    1 == x   );
  BOOST_CHECK( !( 1 != x ) );
  BOOST_CHECK(    2 != x   );
  BOOST_CHECK( !( 2 == x ) );
  
  return 0;
}
