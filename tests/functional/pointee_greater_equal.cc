//
//  pointee_greater_equal のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#include "../../etude/functional/pointee_greater_equal.hpp"

#include "../../etude/types/is_simply_callable.hpp"
#include "../../etude/types/is_less_or_equal_comparable.hpp"
#include <type_traits>
#include <boost/test/minimal.hpp>
#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

// etude::pointee_greater_equal<> の特性
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
  check_basic_traits<etude::pointee_greater_equal<T, U>>();
  check_basic_traits<etude::pointee_greater_equal<U, T>>();
  check_basic_traits<etude::pointee_greater_equal<T&, U&>>();
  check_basic_traits<etude::pointee_greater_equal<U&, T&>>();
  
  // typedef
  STATIC_ASSERT((
    std::is_same<
      typename etude::pointee_greater_equal<T, U>::first_argument_type, T
    >::value
  ));
  STATIC_ASSERT((
    std::is_same<
      typename etude::pointee_greater_equal<T, U>::second_argument_type, U
    >::value
  ));
  STATIC_ASSERT((
    std::is_same<
      typename etude::pointee_greater_equal<U, T>::first_argument_type, U
    >::value
  ));
  STATIC_ASSERT((
    std::is_same<
      typename etude::pointee_greater_equal<U, T>::second_argument_type, T
    >::value
  ));
  // T&
  STATIC_ASSERT((
    std::is_same<
      typename etude::pointee_greater_equal<T&, U&>::first_argument_type, T&
    >::value
  ));
  STATIC_ASSERT((
    std::is_same<
      typename etude::pointee_greater_equal<T&, U&>::second_argument_type, U&
    >::value
  ));
  STATIC_ASSERT((
    std::is_same<
      typename etude::pointee_greater_equal<U&, T&>::first_argument_type, U&
    >::value
  ));
  STATIC_ASSERT((
    std::is_same<
      typename etude::pointee_greater_equal<U&, T&>::second_argument_type, T&
    >::value
  ));
  
  // 実際に比較する
  etude::pointee_greater_equal<>     const greater_equal0 = {};
  etude::pointee_greater_equal<T, U> const greater_equal1 = {};
  etude::pointee_greater_equal<U, T> const greater_equal2 = {};
  etude::pointee_greater_equal<T&, U&> const greater_equal1_ = {};
  etude::pointee_greater_equal<U&, T&> const greater_equal2_ = {};
  
  BOOST_CHECK( greater_equal0( t, u ) == expected1 );
  BOOST_CHECK( greater_equal0( u, t ) == expected2 );
  BOOST_CHECK( greater_equal1( t, u ) == expected1 );
  BOOST_CHECK( greater_equal2( u, t ) == expected2 );
  BOOST_CHECK( greater_equal1_( t, u ) == expected1 );
  BOOST_CHECK( greater_equal2_( u, t ) == expected2 );
  
  // rvalue でも束縛できる
  STATIC_ASSERT((
    etude::is_simply_callable<etude::pointee_greater_equal<>, bool(T&&, U&&)>::value
  ));
  STATIC_ASSERT((
    etude::is_simply_callable<etude::pointee_greater_equal<T, U>, bool(T&&, U&&)>::value
  ));
  STATIC_ASSERT((
    etude::is_simply_callable<etude::pointee_greater_equal<T&, U&>, bool(T&&, U&&)>::value
  ));
  
}

template<class T, class U>
void check_not_comparable( T const&, U const& )
{
  typedef typename etude::pointee<T const&>::type T_;
  typedef typename etude::pointee<U const&>::type U_;
  STATIC_ASSERT((
    !etude::is_less_or_equal_comparable<T_, U_>::value
  ));
  STATIC_ASSERT((
    !etude::is_less_or_equal_comparable<U_, T_>::value
  ));
  
  typedef bool signature1( T const&, U const& );
  typedef bool signature2( U const&, T const& );
  
  STATIC_ASSERT((
    !etude::is_simply_callable<etude::pointee_greater_equal<>, signature1>::value
  ));
  STATIC_ASSERT((
    !etude::is_simply_callable<etude::pointee_greater_equal<T, U>, signature1>::value
  ));
  
  STATIC_ASSERT((
    !etude::is_simply_callable<etude::pointee_greater_equal<>, signature2>::value
  ));
  STATIC_ASSERT((
    !etude::is_simply_callable<etude::pointee_greater_equal<U, T>, signature2>::value
  ));
  
}

#include <boost/optional.hpp>

int test_main( int, char** )
{
  check_basic_traits<etude::pointee_greater_equal<>>();
  
  int i = 0, j = 1;
  int const k = i;
  double d = 3.14;
  
  check( &i, &i, true,  true );
  check( &i, &j, false, true );
  check( &i, &k, true,  true );
  check( &i, &d, false, true );
  check( boost::make_optional(i), boost::make_optional(0.0), true,  true );
  check( boost::make_optional(i), boost::make_optional(1.0), false, true );
  
  void* p = &i;
  check_not_comparable( &p, &i );
  check_not_comparable( &p, boost::make_optional(i) );
  
  return 0;
}
