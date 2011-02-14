//
//  etude の tuple 周りの拡張方法
// 
//    標準ライブラリの tuple 以外でも etude::tuple_xxx 関連を使いたい場合に。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#include "../etude/types/tuple_size.hpp"
#include "../etude/types/tuple_element.hpp"
#include "../etude/utility/tuple_get.hpp"

// adapt させたいクラス
#include <string>

namespace ns
{
  // こいつを tuple として使えるようにする
  struct adapted
  {
    std::string s;
    int i;
    int& ref;
  };
}
// それには
// etude::tuple_size と etude::tuple_element を定義する
namespace etude
{
  // cv 修飾版、参照版は自動で定義される
  template<>
  struct tuple_size<ns::adapted>
    : std::integral_constant<std::size_t, 3> {};
  
  // tuple_element は、必要な型と取得用の static メンバ関数 get を用意
  template<>
  struct tuple_element<0, ns::adapted>
  {
    typedef std::string type;
    
    // 非 const と
    static type& get( ns::adapted& x ) {
      return x.s;
    }
    // const
    static type const& get( ns::adapted const& x ) {
      return x.s;
    }
    
    // rvalue reference は別に要らない
    
  };
  template<>
  struct tuple_element<1, ns::adapted>
  {
    typedef int type;
    
    static type& get( ns::adapted& x ) {
      return x.i;
    }
    static type const& get( ns::adapted const& x ) {
      return x.i;
    }
    
  };
  template<>
  struct tuple_element<2, ns::adapted>
  {
    typedef int& type;
    
    // const だけあれば済む場合もあり
    static type get( ns::adapted const& x ) {
      return x.ref;
    }
    
  };
  
}


// 以上。 試してみる
#include <iostream>
#include <type_traits>
#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

void test1()
{
  STATIC_ASSERT(( etude::tuple_size<ns::adapted&>::value == 3 ));
  
  STATIC_ASSERT((
    std::is_same<
      etude::tuple_element<1, ns::adapted>::type,
      int
    >::value
  ));
  
  int i = 0;
  ns::adapted t = { "hoge", 0, i };
  
  // move できてる？
  STATIC_ASSERT((
    std::is_same<
      decltype( etude::tuple_get<0>( std::move(t) ) ),
      std::string &&
    >::value
  ));
}

int main()
{
  test1();
}
