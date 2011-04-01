//
//  pointee_not_equal_to のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#include "../../etude/functional/pointee_not_equal_to.hpp"

#include "../../etude/types/is_simply_callable.hpp"
#include "../../etude/types/is_equality_comparable.hpp"
#include <type_traits>
#include <boost/test/minimal.hpp>
#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

// etude::pointee_not_equal_to<> の特性
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
  check_basic_traits<etude::pointee_not_equal_to<T, U>>();
  check_basic_traits<etude::pointee_not_equal_to<U, T>>();
  check_basic_traits<etude::pointee_not_equal_to<T&, U&>>();
  check_basic_traits<etude::pointee_not_equal_to<U&, T&>>();
  
  // typedef
  STATIC_ASSERT((
    std::is_same<
      typename etude::pointee_not_equal_to<T, U>::first_argument_type, T
    >::value
  ));
  STATIC_ASSERT((
    std::is_same<
      typename etude::pointee_not_equal_to<T, U>::second_argument_type, U
    >::value
  ));
  STATIC_ASSERT((
    std::is_same<
      typename etude::pointee_not_equal_to<U, T>::first_argument_type, U
    >::value
  ));
  STATIC_ASSERT((
    std::is_same<
      typename etude::pointee_not_equal_to<U, T>::second_argument_type, T
    >::value
  ));
  // T&
  STATIC_ASSERT((
    std::is_same<
      typename etude::pointee_not_equal_to<T&, U&>::first_argument_type, T&
    >::value
  ));
  STATIC_ASSERT((
    std::is_same<
      typename etude::pointee_not_equal_to<T&, U&>::second_argument_type, U&
    >::value
  ));
  STATIC_ASSERT((
    std::is_same<
      typename etude::pointee_not_equal_to<U&, T&>::first_argument_type, U&
    >::value
  ));
  STATIC_ASSERT((
    std::is_same<
      typename etude::pointee_not_equal_to<U&, T&>::second_argument_type, T&
    >::value
  ));
  
  // 実際に比較する
  etude::pointee_not_equal_to<>     const not_equal_to0 = {};
  etude::pointee_not_equal_to<T, U> const not_equal_to1 = {};
  etude::pointee_not_equal_to<U, T> const not_equal_to2 = {};
  etude::pointee_not_equal_to<T&, U&> const not_equal_to1_ = {};
  etude::pointee_not_equal_to<U&, T&> const not_equal_to2_ = {};
  
  BOOST_CHECK( not_equal_to0( t, u ) == expected );
  BOOST_CHECK( not_equal_to0( u, t ) == expected );
  BOOST_CHECK( not_equal_to1( t, u ) == expected );
  BOOST_CHECK( not_equal_to2( u, t ) == expected );
  BOOST_CHECK( not_equal_to1_( t, u ) == expected );
  BOOST_CHECK( not_equal_to2_( u, t ) == expected );
  
  // rvalue でも束縛できる
  STATIC_ASSERT((
    etude::is_simply_callable<etude::pointee_not_equal_to<>, bool(T&&, U&&)>::value
  ));
  STATIC_ASSERT((
    etude::is_simply_callable<etude::pointee_not_equal_to<T, U>, bool(T&&, U&&)>::value
  ));
  STATIC_ASSERT((
    etude::is_simply_callable<etude::pointee_not_equal_to<T&, U&>, bool(T&&, U&&)>::value
  ));
  
}

template<class T, class U>
void check_not_comparable( T const&, U const& )
{
  typedef typename etude::pointee<T const&>::type T_;
  typedef typename etude::pointee<U const&>::type U_;
  STATIC_ASSERT((
    !etude::is_equality_comparable<T_, U_>::value
  ));
  STATIC_ASSERT((
    !etude::is_equality_comparable<U_, T_>::value
  ));
  
  typedef bool signature1( T const&, U const& );
  typedef bool signature2( U const&, T const& );
  
  STATIC_ASSERT((
    !etude::is_simply_callable<etude::pointee_not_equal_to<>, signature1>::value
  ));
  STATIC_ASSERT((
    !etude::is_simply_callable<etude::pointee_not_equal_to<T, U>, signature1>::value
  ));
  
  STATIC_ASSERT((
    !etude::is_simply_callable<etude::pointee_not_equal_to<>, signature2>::value
  ));
  STATIC_ASSERT((
    !etude::is_simply_callable<etude::pointee_not_equal_to<U, T>, signature2>::value
  ));
  
}

#include <boost/optional.hpp>

int test_main( int, char** )
{
  check_basic_traits<etude::pointee_not_equal_to<>>();
  
  int i = 0, j = 1;
  int const k = i;
  double d = 3.14;
  
  check( &i, &i, false );
  check( &i, &j, true );
  check( &i, &k, false );
  check( &i, &d, true );
  check( boost::make_optional(i), boost::make_optional(0.0), false );
  check( boost::make_optional(i), boost::make_optional(1.0), true );
  
  void* p = &i;
  check_not_comparable( &p, &i );
  check_not_comparable( &p, boost::make_optional(i) );
  
  return 0;
}
