//
//  tuple_convertible のテストです。
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/types/tuple_convertible.hpp"

#include <type_traits>

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )


#include <utility>

int main()
{
  // タプル以外の場合
  STATIC_ASSERT(( !etude::tuple_convertible<void, void>::value ));
  STATIC_ASSERT(( !etude::tuple_convertible<int, int>::value ));
  STATIC_ASSERT(( !etude::tuple_convertible<std::tuple<int>, int>::value ));
  STATIC_ASSERT(( !etude::tuple_convertible<int, std::tuple<int>>::value ));

  // 基本的なチェック
  STATIC_ASSERT(( etude::tuple_convertible<std::tuple<int>, std::tuple<int>>::value ));
  
  STATIC_ASSERT(( 
    etude::tuple_convertible<std::tuple<int&, double>, std::tuple<int&, double>>::value
  ));
  // pair とか
  STATIC_ASSERT(( 
    etude::tuple_convertible<
      std::tuple<int&, float>, std::pair<int const&, double>
    >::value
  ));
  STATIC_ASSERT(( 
    !etude::tuple_convertible<std::tuple<int&, float>, std::tuple<char*, float>>::value
  ));
  STATIC_ASSERT(( 
    !etude::tuple_convertible<std::pair<int&, char*>, std::tuple<int&, float>>::value
  ));
  
  // 数違い
  STATIC_ASSERT(( 
    !etude::tuple_convertible<
      std::tuple<int&, int&, int&>, std::tuple<int&>
    >::value
  ));
  STATIC_ASSERT(( 
    !etude::tuple_convertible<
      std::tuple<int&, int&, int&>, std::pair<int&, int&>
    >::value
  ));
  STATIC_ASSERT(( 
    !etude::tuple_convertible<
      std::tuple<>, std::tuple<int&, void, char*>
    >::value
  ));
  
  // 参照
  // From が参照の場合にはよろしくやってくれる
  STATIC_ASSERT(( 
    etude::tuple_convertible< std::tuple<>&, std::tuple<> >::value
  ));
  STATIC_ASSERT(( 
    etude::tuple_convertible< std::tuple<int>&, std::tuple<int&> >::value
  ));
  // To が参照の場合には常時エラー
  STATIC_ASSERT(( 
    !etude::tuple_convertible< std::tuple<>&, std::tuple<>& >::value
  ));
  STATIC_ASSERT(( 
    !etude::tuple_convertible< std::tuple<int>&&, std::tuple<int>&& >::value
  ));

}
