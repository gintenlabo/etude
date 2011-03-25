//
//  identity_less のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#include "../../etude/functional/identity_less.hpp"

#include "../../etude/types/is_simply_callable.hpp"
#include "../../etude/types/is_less_than_comparable.hpp"
#include "../../etude/utility/less_pointer.hpp"
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
void check( T& t, U& u, bool expected1, bool expected2 )
{
  // 型特性
  check_basic_traits<etude::identity_less<T, U>>();
  check_basic_traits<etude::identity_less<U, T>>();
  check_basic_traits<etude::identity_less<T&, U&>>();
  check_basic_traits<etude::identity_less<U&, T&>>();
  
  // typedef
  STATIC_ASSERT((
    std::is_same<
      typename etude::identity_less<T, U>::first_argument_type, T const volatile&
    >::value
  ));
  STATIC_ASSERT((
    std::is_same<
      typename etude::identity_less<T, U>::second_argument_type, U const volatile&
    >::value
  ));
  STATIC_ASSERT((
    std::is_same<
      typename etude::identity_less<U, T>::first_argument_type, U const volatile&
    >::value
  ));
  STATIC_ASSERT((
    std::is_same<
      typename etude::identity_less<U, T>::second_argument_type, T const volatile&
    >::value
  ));
  // T&
  STATIC_ASSERT((
    std::is_same<
      typename etude::identity_less<T&, U&>::first_argument_type, T&
    >::value
  ));
  STATIC_ASSERT((
    std::is_same<
      typename etude::identity_less<T&, U&>::second_argument_type, U&
    >::value
  ));
  STATIC_ASSERT((
    std::is_same<
      typename etude::identity_less<U&, T&>::first_argument_type, U&
    >::value
  ));
  STATIC_ASSERT((
    std::is_same<
      typename etude::identity_less<U&, T&>::second_argument_type, T&
    >::value
  ));
  
  // 実際に比較する
  etude::identity_less<>     const equal_to0 = {};
  etude::identity_less<T, U> const equal_to1 = {};
  etude::identity_less<U, T> const equal_to2 = {};
  etude::identity_less<T&, U&> const equal_to1_ = {};
  etude::identity_less<U&, T&> const equal_to2_ = {};
  
  BOOST_CHECK( equal_to0( t, u ) == expected1 );
  BOOST_CHECK( equal_to0( u, t ) == expected2 );
  BOOST_CHECK( equal_to1( t, u ) == expected1 );
  BOOST_CHECK( equal_to2( u, t ) == expected2 );
  BOOST_CHECK( equal_to1_( t, u ) == expected1 );
  BOOST_CHECK( equal_to2_( u, t ) == expected2 );
  
  // rvalue は束縛できない
  STATIC_ASSERT((
    !etude::is_simply_callable<etude::identity_less<>, bool(T&&, U&&)>::value
  ));
  STATIC_ASSERT((
    !etude::is_simply_callable<etude::identity_less<T, U>, bool(T&&, U&&)>::value
  ));
  STATIC_ASSERT((
    !etude::is_simply_callable<etude::identity_less<T&, U&>, bool(T&&, U&&)>::value
  ));
  
}

template<class T, class U>
void check_not_comparable( T&, U& )
{
  STATIC_ASSERT((
    !etude::is_less_than_comparable<T*, U*>::value
  ));
  STATIC_ASSERT((
    !etude::is_less_than_comparable<U*, T*>::value
  ));
  
  typedef bool signature1( T const&, U const& );
  typedef bool signature2( U const&, T const& );
  
  STATIC_ASSERT((
    !etude::is_simply_callable<etude::identity_less<>, signature1>::value
  ));
  STATIC_ASSERT((
    !etude::is_simply_callable<etude::identity_less<T, U>, signature1>::value
  ));
  
  STATIC_ASSERT((
    !etude::is_simply_callable<etude::identity_less<>, signature2>::value
  ));
  STATIC_ASSERT((
    !etude::is_simply_callable<etude::identity_less<U, T>, signature2>::value
  ));
  
}

struct Base {};
struct Derived : Base {};

int test_main( int, char** )
{
  check_basic_traits<etude::identity_less<>>();
  
  int i = 0;
  int const j = 0;
  int const& r = i;
  double d = 3.14;
  
  bool const less_ij = etude::less_pointer( &i, &j );
  
  check( i, i, false, false );
  check( i, j, less_ij, !less_ij );
  check( i, r, false, false );
  check_not_comparable( i, d );
  
  Derived x, y;
  Base& b = x;
  bool const less_xy = etude::less_pointer( &x, &y );
  check( x, x, false, false );
  check( x, y, less_xy, !less_xy );
  check( b, x, false, false );
  check( b, y, less_xy, !less_xy );
  
  return 0;
}
