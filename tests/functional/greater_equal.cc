//
//  greater_equal のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#include "../../etude/functional/greater_equal.hpp"

#include "../../etude/types/is_simply_callable.hpp"
#include "../../etude/types/is_less_or_equal_comparable.hpp"
#include <type_traits>
#include <boost/test/minimal.hpp>
#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

// etude::less<> の特性
template<class Tested>
void check_basic_traits()
{
  STATIC_ASSERT(( std::is_trivial<Tested>::value ));
  STATIC_ASSERT(( std::is_empty<Tested>::value ));
  STATIC_ASSERT(( std::is_same<typename Tested::result_type, bool>::value ));
}

template<class T, class U>
void check( T const& t, U const& u, bool expected1, bool expected2 )
{
  // 型特性
  check_basic_traits<etude::greater_equal<T, U>>();
  check_basic_traits<etude::greater_equal<U, T>>();
  
  // typedef
  STATIC_ASSERT((
    std::is_same<
      typename etude::greater_equal<T, U>::first_argument_type, T
    >::value
  ));
  STATIC_ASSERT((
    std::is_same<
      typename etude::greater_equal<T, U>::second_argument_type, U
    >::value
  ));
  STATIC_ASSERT((
    std::is_same<
      typename etude::greater_equal<U, T>::first_argument_type, U
    >::value
  ));
  STATIC_ASSERT((
    std::is_same<
      typename etude::greater_equal<U, T>::second_argument_type, T
    >::value
  ));
  
  // 実際に比較する
  etude::greater_equal<>     const comp0 = {};
  etude::greater_equal<T, U> const comp1 = {};
  etude::greater_equal<U, T> const comp2 = {};
  
  BOOST_CHECK( comp0( t, u ) == expected1 );
  BOOST_CHECK( comp0( u, t ) == expected2 );
  BOOST_CHECK( comp1( t, u ) == expected1 );
  BOOST_CHECK( comp2( u, t ) == expected2 );
  
  // <= かつ >= なら ==
  if( expected1 && expected2 ) {
    BOOST_CHECK( t == u && u == t );
  }
}

template<class T, class U>
void check_not_comparable( T const&, U const& )
{
  STATIC_ASSERT((
    !etude::is_less_or_equal_comparable<T, U>::value
  ));
  STATIC_ASSERT((
    !etude::is_less_or_equal_comparable<U, T>::value
  ));
  
  typedef bool signature1( T const&, U const& );
  typedef bool signature2( U const&, T const& );
  
  STATIC_ASSERT((
    !etude::is_simply_callable<etude::greater_equal<>, signature1>::value
  ));
  STATIC_ASSERT((
    !etude::is_simply_callable<etude::greater_equal<T, U>, signature1>::value
  ));
  
  STATIC_ASSERT((
    !etude::is_simply_callable<etude::greater_equal<>, signature2>::value
  ));
  STATIC_ASSERT((
    !etude::is_simply_callable<etude::greater_equal<U, T>, signature2>::value
  ));
  
}

int test_main( int, char** )
{
  check_basic_traits<etude::greater_equal<>>();
  
  int i = 0;
  int const j = 1;
  double d = 3.14;
  
  check( i, 0, true, true );
  check( i, j, false, true );
  check( i, d, false, true );
  
  void* const vp = 0;
  bool const less_ij = etude::less_pointer( &i, &j );
  bool const less_i0 = etude::less_pointer( &i, vp );
  check( &i, &i, true, true );
  check( &i, &j, !less_ij, less_ij );
  check( &i, vp, !less_i0, less_i0 );
  
  check_not_comparable( &i, i );
  check_not_comparable( &j, &d );
  
  return 0;
}
