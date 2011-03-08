//
//  strictly_ordered のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#include "../../etude/operators/strictly_ordered.hpp"

#include <boost/test/minimal.hpp>
#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

struct X
  : private etude::strictly_ordered<
      X, int,
      etude::strictly_ordered<X>
    >
{
  int x;
  explicit X( int x_ ) : x(x_) {}
  
  friend bool operator<( X const& lhs, X const& rhs ) {
    return lhs.x <  rhs.x;
  }
  
  friend bool operator<( X const& lhs, int rhs ) {
    return lhs.x <  rhs;
  }
  friend bool operator>( X const& lhs, int rhs ) {
    return lhs.x >  rhs;
  }
  
};

template<class T, class U>
void check( T const& t, U const& u )
{
  BOOST_CHECK( ( t < u ) == ( u > t ) );
  BOOST_CHECK( ( t > u ) == ( u < t ) );
  
  BOOST_CHECK( ( t <= u ) == ( u >= t ) );
  BOOST_CHECK( ( t >= u ) == ( u <= t ) );
  
  BOOST_CHECK( ( t < u ) == !( t >= u ) );
  BOOST_CHECK( ( t > u ) == !( t <= u ) );
  
}

int test_main( int, char** )
{
  X x(0), y(1), z(2);
  
  check( x, x );
  check( x, y );
  check( x, z );
  
  check( x, 0 );
  check( y, 0 );
  check( z, 0 );
  
  check( 1, x );
  check( 1, y );
  check( 1, z );
  
  return 0;
}
