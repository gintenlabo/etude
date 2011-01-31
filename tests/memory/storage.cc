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
#include <algorithm>
#include <vector>
#include <boost/assert.hpp>

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

#include "../../etude/types/storage_size.hpp"
#include "../../etude/types/storage_align.hpp"

// 基礎的な性質に対するチェック
template<class... Ts>
void basic_check()
{
  // まずメタ関数 etude::storage_of<Ts...> について
  
  // storage_of は純粋なメタ関数
  STATIC_ASSERT(( std::is_empty<etude::storage_of<Ts...>>::value ));
  STATIC_ASSERT(( std::is_trivial<etude::storage_of<Ts...>>::value ));
  
  // type 以外の特性。
  std::size_t const size = etude::storage_of<Ts...>::size;
  std::size_t const align = etude::storage_of<Ts...>::align;
  bool const is_empty = etude::storage_of<Ts...>::is_empty;
  
  if( sizeof...(Ts) != 0 ) {
    // size, align は与えられた型の中での最大値
    std::vector<std::size_t> const sizes = { etude::storage_size<Ts>::value... };
    BOOST_ASSERT(( *std::max_element( sizes.begin(), sizes.end() ) == size ));
    
    std::vector<std::size_t> const aligns = { etude::storage_align<Ts>::value... };
    BOOST_ASSERT(( *std::max_element( aligns.begin(), aligns.end() ) == align ));
    
    // is_empty は与えられた型が全て empty のとき、かつその時に限り true
    std::vector<bool> const is_emptys = { std::is_empty<Ts>::value... };
    BOOST_ASSERT((
      std::all_of( is_emptys.begin(), is_emptys.end(), [](bool b){ return b; } )
        == is_empty
    ));
  }
  else {
    // 型が与えられてない場合
    BOOST_ASSERT((  size == 1 ));
    BOOST_ASSERT(( align == 1 ));
    BOOST_ASSERT(( is_empty ));
  }
  
  // type は std::aligned_storage<size, align>::type と同じ
  STATIC_ASSERT((
    std::is_same<
      typename std::aligned_storage<size, align>::type,
      typename etude::storage_of<Ts...>::type
    >::value
  ));
  
  
  // 続いて etude::storage
  typedef etude::storage<Ts...> storage_type;
  
  // まずメタ関数として見た場合の etude::storage は
  // etude::storage_of<Ts...>::type と同じもの
  STATIC_ASSERT((
    std::is_same<
      typename storage_type::type,
      typename etude::storage_of<Ts...>::type
    >::value
  ));
  
  // etude::storage<Ts...> 自体の性質
  
  // size と align, is_empty は対応する storage_of のものと一致
  STATIC_ASSERT((  sizeof(storage_type) == size ));
  STATIC_ASSERT(( alignof(storage_type) == align ));
  STATIC_ASSERT(( std::is_empty<storage_type>::value == is_empty ));
  
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


// 単独クラスの場合
template<class T>
void check()
{
  basic_check<T>();
  
  typedef etude::storage<T> storage_type;
  
  // storage_of::size, align, is_empty は単独クラスのそれに一致
  STATIC_ASSERT(( etude::storage_of<T>::size  == etude::storage_size<T>::value ));
  STATIC_ASSERT(( etude::storage_of<T>::align == etude::storage_align<T>::value ));
  STATIC_ASSERT(( etude::storage_of<T>::is_empty == std::is_empty<T>::value ));
  
  // 重複になるが一応
  STATIC_ASSERT(( etude::storage_size<T>::value  ==  sizeof(storage_type) ));
  STATIC_ASSERT(( etude::storage_align<T>::value == alignof(storage_type) ));
  STATIC_ASSERT(( std::is_empty<T>::value == std::is_empty<storage_type>::value ));
  
  // 参照でない場合、 storage_size<T>::value は sizeof(T) に等しい
  STATIC_ASSERT(( std::is_reference<T>::value || etude::storage_size<T>::value  ==  sizeof(T) ));
  STATIC_ASSERT(( std::is_reference<T>::value || etude::storage_align<T>::value == alignof(T) ));
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
  check<void* const>();
  
  // trivial class
  check<trivial_empty_class>();
  check<non_trivial_empty_class>();
  
  // 普通のクラス
  check<std::string>();
  check<std::shared_ptr<int>>();
  check<std::fstream>();
  
  // 複数の型に対して
  
  // まず空の storage
  basic_check<>();
  typedef etude::storage<> empty_storage;
  STATIC_ASSERT(( std::is_empty<empty_storage>::value ));
  
  // 複数の型を格納した storage
  basic_check<char, int, long>();
  typedef etude::storage<char, int, long> storage3;
  STATIC_ASSERT(( !std::is_empty<storage3>::value ));
  STATIC_ASSERT(( sizeof(storage3) == sizeof(long) ));  // sizeof(int) <= sizeof(long)
  STATIC_ASSERT(( alignof(storage3) == alignof(long) ));  // これは実のところよく分からないが、たぶんあってる
}
