//
//  etude/memory/is_typed_in_place_factory.hpp に対するテスト
//
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#include "../../etude/memory/is_typed_in_place_factory.hpp"

// #include <boost/test/minimal.hpp>  // うごかない…。
#include <boost/assert.hpp>
#include <boost/type.hpp>
#include <boost/utility/typed_in_place_factory.hpp>

// typed in-place に associated された型があるなら true を返す
template<class T, class = typename etude::typed_in_place_associated<T>::type>
bool has_typed_in_place_associated_type( boost::type<T> ) {
  return true;
}
bool has_typed_in_place_associated_type( ... ) {
  return false;
}

template<typename T>
void check_is_typed_in_place()
{
  BOOST_ASSERT( etude::is_typed_in_place_factory<T>::value == true );
  BOOST_ASSERT( etude::is_typed_in_place_factory<T&>::value == true );
  BOOST_ASSERT( etude::is_typed_in_place_factory<T&&>::value == true );
  BOOST_ASSERT( etude::is_typed_in_place_factory<T const>::value == true );
  BOOST_ASSERT( etude::is_typed_in_place_factory<T const&>::value == true );
  
  BOOST_ASSERT( has_typed_in_place_associated_type( boost::type<T>() ) == true );
  BOOST_ASSERT( has_typed_in_place_associated_type( boost::type<T&>() ) == true );
}
template<typename T>
void check_is_not_typed_in_place()
{
  BOOST_ASSERT( etude::is_typed_in_place_factory<T>::value == false );
  BOOST_ASSERT( etude::is_typed_in_place_factory<T&>::value == false );
  BOOST_ASSERT( etude::is_typed_in_place_factory<T&&>::value == false );
  BOOST_ASSERT( etude::is_typed_in_place_factory<T const>::value == false );
  BOOST_ASSERT( etude::is_typed_in_place_factory<T const&>::value == false );
  
  BOOST_ASSERT( has_typed_in_place_associated_type( boost::type<T>() ) == false );
  BOOST_ASSERT( has_typed_in_place_associated_type( boost::type<T&>() ) == false );
}

#include <type_traits>

int main()
{
  typedef decltype( boost::in_place<int>() ) typed_in_place_type_1;
  typedef decltype( boost::in_place<double>( 1 ) ) typed_in_place_type_2;
  struct typed_in_place_type_3 : boost::typed_in_place_factory_base {
    typedef typed_in_place_type_3 value_type;
  };
  check_is_typed_in_place<typed_in_place_type_1>();
  check_is_typed_in_place<typed_in_place_type_2>();
  check_is_typed_in_place<typed_in_place_type_3>();
  
  // typed_in_place_associated のチェック
  BOOST_ASSERT(( std::is_same<
    etude::typed_in_place_associated<typed_in_place_type_1>::type, int
  >::value ));
  BOOST_ASSERT(( std::is_same<
    etude::typed_in_place_associated<typed_in_place_type_2>::type, double
  >::value ));
  BOOST_ASSERT(( std::is_same<
    etude::typed_in_place_associated<typed_in_place_type_3>::type, typed_in_place_type_3
  >::value ));
  
  class klass{};
  check_is_not_typed_in_place<int>();
  check_is_not_typed_in_place<char*>();
  check_is_not_typed_in_place<klass>();
}
