//
//  etude/memory/storage.hpp に対するテスト
//    というより仕様
//
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#include "../../etude/memory/storage.hpp"

#include <type_traits>
#include <boost/assert.hpp>

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

template<class T>
void check()
{
  typedef etude::storage<T> storage_type;
  
  // まずメタ関数として見た場合の etude::storage は
  // std::aligned_storage<sizeof(T), alignof(T)>::type と同じもの
  STATIC_ASSERT((
    std::is_same<
      typename std::aligned_storage<sizeof(T), alignof(T)>::type,
      typename storage_type::type
    >::value
  ));
  
  // etude::storage<T> 自体の性質
  
  // size と align が一致
  STATIC_ASSERT((  sizeof(T) ==  sizeof(storage_type) ));
  STATIC_ASSERT(( alignof(T) == alignof(storage_type) ));
  // T が empty ならば etude::storage<T> も empty
  STATIC_ASSERT(( std::is_empty<T>::value == std::is_empty<storage_type>::value ));
  
  // standard layout class
  STATIC_ASSERT(( std::is_standard_layout<storage_type>::value ));
  // trivially default-constructible class
  STATIC_ASSERT(( std::has_trivial_default_constructor<storage_type>::value ));
  // trivially destructible class
  STATIC_ASSERT(( std::has_trivial_destructor<storage_type>::value ));
  
  /*
  // non-copyable class
  STATIC_ASSERT(( !std::is_copy_constructible<storage_type>::value ));
  STATIC_ASSERT(( !std::is_move_constructible<storage_type>::value ));
  STATIC_ASSERT(( !std::is_copy_assignable<storage_type>::value ));
  STATIC_ASSERT(( !std::is_move_assignable<storage_type>::value ));
  */
  
  // address のテスト
  storage_type storage;
  storage_type const const_storage = {};
  
  // 型は void*, void const*
  STATIC_ASSERT((
    std::is_same<
      decltype( storage.address() ), void*
    >::value
  ));
  STATIC_ASSERT((
    std::is_same<
      decltype( const_storage.address() ), void const*
    >::value
  ));
  
  // 戻り値は storage 自体のアドレスに等しい
  BOOST_ASSERT( static_cast<void*>(&storage) == storage.address() );
  BOOST_ASSERT( static_cast<void const*>(&const_storage) == const_storage.address() );

}

// テスト用クラス
struct trivial_empty_class {};
STATIC_ASSERT( std::is_empty<trivial_empty_class>::value );
STATIC_ASSERT( std::is_trivial<trivial_empty_class>::value );

struct non_trivial_empty_class
{
  non_trivial_empty_class() {}
  ~non_trivial_empty_class() {}
};
STATIC_ASSERT( std::is_empty<non_trivial_empty_class>::value );
STATIC_ASSERT( !std::is_trivial<non_trivial_empty_class>::value );

#include <string>
#include <memory>
#include <fstream>

int main()
{
  // 組み込み型
  check<int>();
  check<char*>();
  check<double&>();
  
  // trivial class
  check<trivial_empty_class>();
  check<non_trivial_empty_class>();
  
  // 普通のクラス
  check<std::string>();
  check<std::shared_ptr<int>>();
  check<std::fstream>();
}
