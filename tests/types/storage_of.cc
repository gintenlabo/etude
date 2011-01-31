//
//  storage_of のテストです。
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/types/storage_of.hpp"

#include <vector>
#include <algorithm>
#include <type_traits>
#include <boost/assert.hpp>

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

template<class... Ts>
inline void basic_check()
{
  typedef etude::storage_of<Ts...> storage_of_Ts;
  
  // storage_of は純粋なメタ関数
  STATIC_ASSERT(( std::is_empty<storage_of_Ts>::value ));
  STATIC_ASSERT(( std::is_trivial<storage_of_Ts>::value ));
  
  // type の他に定数 size, align, is_empty を持つ
  static std::size_t const size  = storage_of_Ts::size;
  static std::size_t const align = storage_of_Ts::align;
  static bool const is_empty = storage_of_Ts::is_empty;
  
  // それぞれの意味は以下の通り：
  if( sizeof...(Ts) != 0 ) {
    // size, align は与えられた型の中での最大値
    std::vector<std::size_t> const sizes = { etude::storage_size<Ts>::value... };
    BOOST_ASSERT(( *std::max_element( sizes.begin(), sizes.end() ) == size ));
    
    std::vector<std::size_t> const aligns = { etude::storage_align<Ts>::value... };
    BOOST_ASSERT(( *std::max_element( aligns.begin(), aligns.end() ) == align ));
    
    // is_empty は与えられた型が全て empty のとき、かつその時に限り true
    std::vector<bool> const is_emptys = { std::is_empty<Ts>::value... };
    BOOST_ASSERT((
      std::all_of( is_emptys.begin(), is_emptys.end(),
        [](bool b){ return b; } ) == is_empty
    ));
  }
  else {
    // 型が与えられてない場合
    // size, align は 0
    BOOST_ASSERT((  size == 1 ));
    BOOST_ASSERT(( align == 1 ));
    // is_empty は true
    BOOST_ASSERT(( is_empty ));
  }
  
  // type は std::aligned_storage<size, align>::type と同じ
  STATIC_ASSERT((
    std::is_same<
      typename std::aligned_storage<size, align>::type,
      typename storage_of_Ts::type
    >::value
  ));
  // こいつは POD
  STATIC_ASSERT(( std::is_pod<typename storage_of_Ts::type>::value ));
}

// 型がひとつの場合は
template<class T>
inline void check()
{
  basic_check<T>();
  
  // 特に storage_of::size, align, is_empty はそれぞれ以下のようになる
  STATIC_ASSERT(( etude::storage_of<T>::size  == etude::storage_size<T>::value ));
  STATIC_ASSERT(( etude::storage_of<T>::align == etude::storage_align<T>::value ));
  STATIC_ASSERT(( etude::storage_of<T>::is_empty == std::is_empty<T>::value ));
  
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
