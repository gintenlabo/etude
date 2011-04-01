//
//  pointee_less のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#include "../../etude/functional/pointee_less.hpp"

#include "../../etude/types/is_simply_callable.hpp"
#include "../../etude/types/is_less_than_comparable.hpp"
#include <type_traits>
#include <boost/test/minimal.hpp>
#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

// etude::pointee_less<> の特性
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
  check_basic_traits<etude::pointee_less<T, U>>();
  check_basic_traits<etude::pointee_less<U, T>>();
  check_basic_traits<etude::pointee_less<T&, U&>>();
  check_basic_traits<etude::pointee_less<U&, T&>>();
  
  // typedef
  STATIC_ASSERT((
    std::is_same<
      typename etude::pointee_less<T, U>::first_argument_type, T
    >::value
  ));
  STATIC_ASSERT((
    std::is_same<
      typename etude::pointee_less<T, U>::second_argument_type, U
    >::value
  ));
  STATIC_ASSERT((
    std::is_same<
      typename etude::pointee_less<U, T>::first_argument_type, U
    >::value
  ));
  STATIC_ASSERT((
    std::is_same<
      typename etude::pointee_less<U, T>::second_argument_type, T
    >::value
  ));
  // T&
  STATIC_ASSERT((
    std::is_same<
      typename etude::pointee_less<T&, U&>::first_argument_type, T&
    >::value
  ));
  STATIC_ASSERT((
    std::is_same<
      typename etude::pointee_less<T&, U&>::second_argument_type, U&
    >::value
  ));
  STATIC_ASSERT((
    std::is_same<
      typename etude::pointee_less<U&, T&>::first_argument_type, U&
    >::value
  ));
  STATIC_ASSERT((
    std::is_same<
      typename etude::pointee_less<U&, T&>::second_argument_type, T&
    >::value
  ));
  
  // 実際に比較する
  etude::pointee_less<>     const less0 = {};
  etude::pointee_less<T, U> const less1 = {};
  etude::pointee_less<U, T> const less2 = {};
  etude::pointee_less<T&, U&> const less1_ = {};
  etude::pointee_less<U&, T&> const less2_ = {};
  
  BOOST_CHECK( less0( t, u ) == expected1 );
  BOOST_CHECK( less0( u, t ) == expected2 );
  BOOST_CHECK( less1( t, u ) == expected1 );
  BOOST_CHECK( less2( u, t ) == expected2 );
  BOOST_CHECK( less1_( t, u ) == expected1 );
  BOOST_CHECK( less2_( u, t ) == expected2 );
  
  // rvalue でも束縛できる
  STATIC_ASSERT((
    etude::is_simply_callable<etude::pointee_less<>, bool(T&&, U&&)>::value
  ));
  STATIC_ASSERT((
    etude::is_simply_callable<etude::pointee_less<T, U>, bool(T&&, U&&)>::value
  ));
  STATIC_ASSERT((
    etude::is_simply_callable<etude::pointee_less<T&, U&>, bool(T&&, U&&)>::value
  ));
  
}

template<class T, class U>
void check_not_comparable( T const&, U const& )
{
  typedef typename etude::pointee<T const&>::type T_;
  typedef typename etude::pointee<U const&>::type U_;
  STATIC_ASSERT((
    !etude::is_less_than_comparable<T_, U_>::value
  ));
  STATIC_ASSERT((
    !etude::is_less_than_comparable<U_, T_>::value
  ));
  
  typedef bool signature1( T const&, U const& );
  typedef bool signature2( U const&, T const& );
  
  STATIC_ASSERT((
    !etude::is_simply_callable<etude::pointee_less<>, signature1>::value
  ));
  STATIC_ASSERT((
    !etude::is_simply_callable<etude::pointee_less<T, U>, signature1>::value
  ));
  
  STATIC_ASSERT((
    !etude::is_simply_callable<etude::pointee_less<>, signature2>::value
  ));
  STATIC_ASSERT((
    !etude::is_simply_callable<etude::pointee_less<U, T>, signature2>::value
  ));
  
}

#include <boost/optional.hpp>

int test_main( int, char** )
{
  check_basic_traits<etude::pointee_less<>>();
  
  int i = 0, j = 1;
  int const k = i;
  double d = 3.14;
  
  check( &i, &i, false, false );
  check( &i, &j, true,  false );
  check( &i, &k, false, false );
  check( &i, &d, true,  false );
  check( boost::make_optional(i), boost::make_optional(0.0), false, false );
  check( boost::make_optional(i), boost::make_optional(1.0), true,  false );
  
  void* p = &i;
  check_not_comparable( &p, &i );
  check_not_comparable( &p, boost::make_optional(i) );
  
  return 0;
}
