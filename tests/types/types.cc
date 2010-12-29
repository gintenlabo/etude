//
//  type/types のテストです。
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/types/types.hpp"

#include <type_traits>
#include <tuple>

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

// etude::type をチェックする
template<class T>
void check_traits( etude::type<T> )
{
  typedef etude::type<T> type;
  
  // 基本的な性質のみ
  STATIC_ASSERT(( std::is_empty<type>::value ));
  STATIC_ASSERT(( std::is_trivial<type>::value ));
}

// etude::types をチェックする
template<class... Types>
void check_traits( etude::types<Types...> )
{
  typedef etude::types<Types...> types_type;
  
  // 基本的な性質
  STATIC_ASSERT(( std::is_empty<types_type>::value ));
  STATIC_ASSERT(( std::is_trivial<types_type>::value ));
  
  // type
  STATIC_ASSERT(( std::is_same<typename types_type::type, types_type>::value ));
  // size
  STATIC_ASSERT(( types_type::size == sizeof...(Types) ));
  
  // apply
  STATIC_ASSERT((
    std::is_same<
      typename types_type::template apply<etude::types>::type,
      types_type
    >::value
  ));
  // 例として、同じパラメータの tuple を作る
  STATIC_ASSERT((
    std::is_same<
      typename types_type::template apply<std::tuple>::type,
      std::tuple<Types...>
    >::value
  ));
}

#include <string>

int main()
{
  // type に対するチェック
  check_traits( etude::type<int>() );
  check_traits( etude::type<char*&>() );
  
  // types に対するチェック
  check_traits( etude::types<>() );
  check_traits( etude::types<int>() );
  check_traits( etude::types<char, int&, void, char>() );
  
  // apply のチェック
  STATIC_ASSERT((
    std::is_same< std::string,
      etude::types<char>::apply<std::basic_string>::type
    >::value
  ));
}
