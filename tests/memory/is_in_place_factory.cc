//
//  etude/memory/is_in_place_factory.hpp に対するテスト
//    仕様書も兼ねる感じで詳細に、かつインクルードするヘッダは最低限。
//
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#include "../../etude/memory/is_in_place_factory.hpp"

// #include <boost/test/minimal.hpp>  // うごかない…。
#include <boost/assert.hpp>
#include <boost/utility/in_place_factory.hpp>

template<typename T>
void check_is_in_place()
{
  BOOST_ASSERT( etude::is_in_place_factory<T>::value == true );
  BOOST_ASSERT( etude::is_in_place_factory<T&>::value == true );
  BOOST_ASSERT( etude::is_in_place_factory<T&&>::value == true );
  BOOST_ASSERT( etude::is_in_place_factory<T const>::value == true );
  BOOST_ASSERT( etude::is_in_place_factory<T const&>::value == true );
}
template<typename T>
void check_is_not_in_place()
{
  BOOST_ASSERT( etude::is_in_place_factory<T>::value == false );
  BOOST_ASSERT( etude::is_in_place_factory<T&>::value == false );
  BOOST_ASSERT( etude::is_in_place_factory<T&&>::value == false );
  BOOST_ASSERT( etude::is_in_place_factory<T const>::value == false );
  BOOST_ASSERT( etude::is_in_place_factory<T const&>::value == false );
}

int main()
{
  typedef decltype( boost::in_place() ) in_place_type_1;
  typedef decltype( boost::in_place( 1 ) ) in_place_type_2;
  struct in_place_type_3 : boost::in_place_factory_base {};
  check_is_in_place<in_place_type_1>();
  check_is_in_place<in_place_type_2>();
  check_is_in_place<in_place_type_3>();
  
  class klass{};
  check_is_not_in_place<int>();
  check_is_not_in_place<char*>();
  check_is_not_in_place<klass>();
}
