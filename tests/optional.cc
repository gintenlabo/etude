//
//  optional のテストです。
//    
//    TODO: ちゃんと書く
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../etude/optional.hpp"

template class etude::optional<int>;
template class etude::optional<int&>;

#include <type_traits>
#include <boost/test/minimal.hpp>

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

#include <boost/utility/addressof.hpp>
template<class T>
inline bool is_same_object( T& x, T& y )
{
  return boost::addressof(x) == boost::addressof(y);
}


#include <boost/none.hpp>
int test_main( int, char** )
{
  etude::optional<int> x;
  BOOST_CHECK( x == boost::none );
  
  int i = 0;
  x = etude::make_optional(i);
  
  BOOST_CHECK( x && *x == 0 && !is_same_object( *x, i ) );
  
  x = 1;
  etude::optional<int> y;
  swap( x, y );
  BOOST_CHECK( !x && y && *y == 1 );
  
  return 0;
}
