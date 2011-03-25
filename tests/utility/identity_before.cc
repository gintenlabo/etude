//
//  identity_before のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/utility/identity_before.hpp"

#include <boost/test/minimal.hpp>
#include <functional>

struct Base {};
struct Derived : Base {};

int test_main( int, char** )
{
  int i = 0, j = 0;
  int const &ri = i;
  BOOST_CHECK( !etude::identity_before( i, i ) );
  BOOST_CHECK(  etude::identity_before( i, j ) == !etude::identity_before( j, i ) );
  BOOST_CHECK( !etude::identity_before( i, ri ) );
  BOOST_CHECK( !etude::identity_before<int>( i, std::ref(i) ) );  // OK
  
  Derived x, y;
  Base& b = x;
  BOOST_CHECK( !etude::identity_before( x, x ) );
  BOOST_CHECK(  etude::identity_before( x, y ) == !etude::identity_before( y, x ) );
  BOOST_CHECK( !etude::identity_before( b, x ) );
  BOOST_CHECK(  etude::identity_before( b, y ) ==  etude::identity_before( x, y ) );
  
  return 0;
}
