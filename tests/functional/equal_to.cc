//
//  equal_to のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#include "../../etude/functional/equal_to.hpp"

#include "../../etude/types/is_simply_callable.hpp"
#include <type_traits>
#include <boost/test/minimal.hpp>
#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

// etude::equal_to<> の特性
template<class Tested>
void check_basic_traits()
{
  STATIC_ASSERT(( std::is_trivial<Tested>::value ));
  STATIC_ASSERT(( std::is_empty<Tested>::value ));
  STATIC_ASSERT(( std::is_same<typename Tested::result_type, bool>::value ));
}

template<class T, class U>
void check( T const& t, U const& u, bool expected )
{
  // 型特性
  check_basic_traits<etude::equal_to<T, U>>();
  check_basic_traits<etude::equal_to<U, T>>();
  
  // typedef
  STATIC_ASSERT((
    std::is_same<
      typename etude::equal_to<T, U>::first_argument_type, T
    >::value
  ));
  STATIC_ASSERT((
    std::is_same<
      typename etude::equal_to<T, U>::second_argument_type, U
    >::value
  ));
  STATIC_ASSERT((
    std::is_same<
      typename etude::equal_to<U, T>::first_argument_type, U
    >::value
  ));
  STATIC_ASSERT((
    std::is_same<
      typename etude::equal_to<U, T>::second_argument_type, T
    >::value
  ));
  
  // 実際に比較する
  etude::equal_to<>     const equal_to0 = {};
  etude::equal_to<T, U> const equal_to1 = {};
  etude::equal_to<U, T> const equal_to2 = {};
  
  BOOST_CHECK( equal_to0( t, u ) == expected );
  BOOST_CHECK( equal_to0( u, t ) == expected );
  BOOST_CHECK( equal_to1( t, u ) == expected );
  BOOST_CHECK( equal_to2( u, t ) == expected );
  
}

template<class T, class U>
void check_not_comparable( T const&, U const& )
{
  STATIC_ASSERT((
    !etude::is_equality_comparable<T, U>::value
  ));
  STATIC_ASSERT((
    !etude::is_equality_comparable<U, T>::value
  ));
  
  typedef bool signature1( T const&, U const& );
  typedef bool signature2( U const&, T const& );
  
  STATIC_ASSERT((
    !etude::is_simply_callable<etude::equal_to<>, signature1>::value
  ));
  STATIC_ASSERT((
    !etude::is_simply_callable<etude::equal_to<T, U>, signature1>::value
  ));
  
  STATIC_ASSERT((
    !etude::is_simply_callable<etude::equal_to<>, signature2>::value
  ));
  STATIC_ASSERT((
    !etude::is_simply_callable<etude::equal_to<U, T>, signature2>::value
  ));
  
}

int test_main( int, char** )
{
  check_basic_traits<etude::equal_to<>>();
  
  int i = 0;
  int const j = 1;
  double d = 3.14;
  
  check( i, 0, true );
  check( i, j, false );
  check( i, d, false );
  
  void* const vp = 0;
  check( &i, &i, true );
  check( &i, &j, false );
  check( &i, vp, false );
  
  check_not_comparable( &i, i );
  check_not_comparable( &j, &d );
  
  return 0;
}
