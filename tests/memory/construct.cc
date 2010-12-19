//
//  etude/memory/operator_delete.hpp に対するテスト
//
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#include "../../etude/memory/construct.hpp"
#include "../../etude/memory/operator_new.hpp"
#include "../../etude/memory/in_place_factory.hpp"
#include "../../etude/memory/typed_in_place_factory.hpp"
#include "test_utilities.hpp"

#include <boost/assert.hpp>
#include <memory>
#include <type_traits>
#include <stdexcept>

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

struct bad_construct
  : std::runtime_error
{
  bad_construct()
    : std::runtime_error("bad construct") {}
};

struct hoge
  : lifetime_check<hoge>
{
  explicit hoge( bool throw_exception = false ) {
    if( throw_exception ) {
      throw bad_construct();
    }
  }
  
  static void* operator new( std::size_t size ) {
    void* const p = ::operator new( size );
    instances_() += 1;
    return p;
  }
  static void operator delete( void* p ) {
    ::operator delete(p);
    instances_() -= 1;
  }
  
  static int count_allocated_instance() {
    return instances_();
  }
  
  int x;
  
 private:
  static int& instances_() {
    static int instances = 0;
    return instances;
  }
  
};

template<class F, class Check>
void check( F f, Check check_after_f )
{
  int const existing_instances = hoge::count_existing_instance();
  int const allocated_instances = hoge::count_allocated_instance();
  
  try {
    auto p0 = f();
    check_after_f( p0.get() );
  }
  catch( bad_construct& ) {
  }
  
  BOOST_ASSERT( hoge::count_existing_instance() == existing_instances );
  BOOST_ASSERT( hoge::count_allocated_instance() == allocated_instances );
}


int main()
{
  // 生ポインタ版
  etude::storage<hoge> buf;
  
  // 通常パス
  check(
    [&](){
      return etude::construct<hoge>( buf.address() );
    },
    []( hoge* p ){
      BOOST_ASSERT( p != 0 );
      BOOST_ASSERT( hoge::count_existing_instance() == 1 );
    }
  );
  // 例外パス
  check(
    [&](){
      return etude::construct<hoge>( buf.address(), true );
    },
    []( hoge* ){
      BOOST_ASSERT( !"should not get here." );
    }
  );
  
  // 通常パス（in place）
  check(
    [&](){
      return etude::construct<hoge>( etude::in_place(), buf.address() );
    },
    []( hoge* p ){
      BOOST_ASSERT( p != 0 );
      BOOST_ASSERT( hoge::count_existing_instance() == 1 );
    }
  );
  // 例外パス( in place )
  check(
    [&](){
      return etude::construct<hoge>( etude::in_place(true), buf.address() );
    },
    []( hoge* ){
      BOOST_ASSERT( !"should not get here." );
    }
  );
  
  // 通常パス（typed in place）
  check(
    [&](){
      return etude::construct( etude::in_place<hoge>(), buf.address() );
    },
    []( hoge* p ){
      BOOST_ASSERT( p != 0 );
      BOOST_ASSERT( hoge::count_existing_instance() == 1 );
    }
  );
  // 例外パス( typed in place )
  check(
    [&](){
      return etude::construct( etude::in_place<hoge>(true), buf.address() );
    },
    []( hoge* ){
      BOOST_ASSERT( !"should not get here." );
    }
  );
  
  // unique_ptr 版
  // 通常パス
  check(
    [&](){
      return etude::construct<hoge>( etude::operator_new<hoge>() );
    },
    []( hoge* p ){
      BOOST_ASSERT( p != 0 );
      BOOST_ASSERT( hoge::count_allocated_instance() == 1 );
      BOOST_ASSERT( hoge::count_existing_instance() == 1 );
    }
  );
  // 例外パス
  check(
    [&](){
      return etude::construct<hoge>( etude::operator_new<hoge>(), true );
    },
    []( hoge* ){
      BOOST_ASSERT( !"should not get here." );
    }
  );
  
  // 通常パス（in place）
  check(
    [&](){
      return etude::construct<hoge>( etude::in_place(),
        etude::operator_new( sizeof(hoge) ) );
    },
    []( hoge* p ){
      BOOST_ASSERT( p != 0 );
      BOOST_ASSERT( hoge::count_allocated_instance() == 0 ); // ::operator new で確保したので
      BOOST_ASSERT( hoge::count_existing_instance() == 1 );
    }
  );
  // 例外パス( in place )
  check(
    [&](){
      return etude::construct<hoge>( etude::in_place(true),
        etude::operator_new( sizeof(hoge) ) );
    },
    []( hoge* ){
      BOOST_ASSERT( !"should not get here." );
    }
  );
  
  // 通常パス（typed in place）
  check(
    [&](){
      return etude::construct( etude::in_place<hoge>(), etude::operator_new<hoge>() );
    },
    []( hoge* p ){
      BOOST_ASSERT( p != 0 );
      BOOST_ASSERT( hoge::count_allocated_instance() == 1 );
      BOOST_ASSERT( hoge::count_existing_instance() == 1 );
    }
  );
  // 例外パス( typed in place )
  check(
    [&](){
      return etude::construct( etude::in_place<hoge>(true),
        etude::operator_new<hoge>() );
    },
    []( hoge* ){
      BOOST_ASSERT( !"should not get here." );
    }
  );
}
