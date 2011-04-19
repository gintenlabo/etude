//
//  less_pointer のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/utility/less_pointer.hpp"

#include <boost/test/minimal.hpp>
#include <type_traits>
#include <utility>

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

template<class T, class U>
inline void check( T* x, U* y )
{
  bool const lt = etude::less_pointer( x, y );
  bool const gt = etude::less_pointer( y, x );
  bool const eq = ( x == y );
  
  // total order になってることをチェック
  if( lt ) {
    BOOST_CHECK( !gt && !eq );
  }
  else if( eq ) {
    BOOST_CHECK( !gt );
  }
  else {
    BOOST_CHECK( gt );
  }
}


struct Base {};
struct Derived : Base {};

int test_main( int, char** )
{
  Derived x, y;
  
  Derived* const p0 = 0;
  Base* const p1 = &x;
  Base* const p2 = &y;
  Derived* const p3 = &x;
  Derived* const p4 = &y;
  
  check( p0, p0 );
  check( p0, p1 );
  check( p0, p3 );
  
  check( p1, p1 );
  check( p1, p2 );
  check( p1, p3 );
  check( p1, p4 );
  
  check( p2, p2 );
  check( p2, p3 );
  check( p2, p4 );
  
  check( p3, p3 );
  check( p3, p4 );
  
  return 0;
}
