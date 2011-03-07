//
//  etude/memory/storage.hpp に対するテスト
//    というより仕様
//
//  Copyright (C) 2010-11  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#include "../../etude/memory/storage.hpp"

#include <type_traits>
#include <boost/assert.hpp>

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

#include "../../etude/types/storage_of.hpp"
#include "../../etude/types/storage_size.hpp"
#include "../../etude/types/storage_align.hpp"

// 基礎的な性質に対するチェック
template<class... Ts>
inline void basic_check()
{
  typedef etude::storage<Ts...> storage_type;
  
  // 各種特性（それぞれの意味は tests/types/storage_of.cc を参照）
  std::size_t const size = etude::storage_of<Ts...>::size;
  std::size_t const align = etude::storage_of<Ts...>::align;
  bool const is_empty = etude::storage_of<Ts...>::is_empty;
  
  // storage<Ts...>::type は std::aligned_storage<size, align>::type と同じ
  STATIC_ASSERT((
    std::is_same<
      typename storage_type::type,
      typename std::aligned_storage<size, align>::type
    >::value
  ));
  // これは etude::storage_of<Ts...>::type とも同じ
  STATIC_ASSERT((
    std::is_same<
      typename storage_type::type,
      typename etude::storage_of<Ts...>::type
    >::value
  ));
  // storage_type::type は POD
  STATIC_ASSERT(( std::is_pod<typename storage_type::type>::value ));
  
  // storage_type は非 POD （コピー不可能）
  STATIC_ASSERT(( !std::is_pod<storage_type>::value ));
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
  
  // storage_type の size と align, is_empty は対応する storage_of のものと一致
  STATIC_ASSERT((  sizeof(storage_type) == size ));
  STATIC_ASSERT(( alignof(storage_type) == align ));
  STATIC_ASSERT(( std::is_empty<storage_type>::value == is_empty ));
  
  
  // address のテスト
  storage_type storage;
  storage_type const const_storage;
  
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


// 単独クラスの場合
template<class T>
inline void check()
{
  basic_check<T>();
  
  typedef etude::storage<T> storage_type;
  // 特に size, align, is_empty に関してはこのような関係がある
  STATIC_ASSERT((  sizeof(storage_type) == etude::storage_size <T>::value ));
  STATIC_ASSERT(( alignof(storage_type) == etude::storage_align<T>::value ));
  STATIC_ASSERT(( std::is_empty<storage_type>::value == std::is_empty<T>::value ));
}

// 複数ある場合は基本チェックだけ
template<class... Ts>
inline typename std::enable_if<sizeof...(Ts) != 1>::type check()
{
  basic_check<Ts...>();
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
  check<char>();
  check<char*>();
  check<double&>();
  check<double&&>();
  check<void* const>();
  
  // trivial class
  check<trivial_empty_class>();
  check<non_trivial_empty_class>();
  
  // 普通のクラス
  check<std::string>();
  check<std::shared_ptr<int> const>();
  check<std::fstream>();
  
  // 複数の型に対して
  
  // 空
  check<>();
  // 複数の型を格納した storage
  basic_check<char, int, long>();
  basic_check<char&, trivial_empty_class, std::string const, std::fstream>();
}
