//
//  identity_equal のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/utility/identity_equal.hpp"

#include <boost/test/minimal.hpp>
#include <functional>

struct Base {};
struct Derived : Base {};

int test_main( int, char** )
{
  int i = 0, j = 0;
  int const &ri = i;
  BOOST_CHECK(  etude::identity_equal( i, i ) );
  BOOST_CHECK( !etude::identity_equal( i, j ) );
  BOOST_CHECK(  etude::identity_equal( i, ri ) );
  // etude::identity_equal( i, 0 );   // ill-formed
  // etude::identity_equal( ri, 0 );  // ill-formed
  // etude::identity_equal( i, std::ref(i) ) ); // ill-formed 
  BOOST_CHECK( etude::identity_equal<int>( i, std::ref(i) ) );  // OK
  
  Derived x, y;
  Base& b = x;
  BOOST_CHECK(  etude::identity_equal( x, x ) );
  BOOST_CHECK( !etude::identity_equal( x, y ) );
  BOOST_CHECK(  etude::identity_equal( b, x ) );
  BOOST_CHECK( !etude::identity_equal( b, y ) );
  
  return 0;
}
