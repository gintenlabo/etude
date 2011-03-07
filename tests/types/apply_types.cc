//
//  apply_types のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/types/apply_types.hpp"

#include <type_traits>

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

// チェック用関数
template< template<class...> class Template, class Types, class Expected >
void check()
{
  typedef typename etude::apply_types<Template, Types>::type type;
  
  STATIC_ASSERT(( std::is_same<type, Expected>::value ));
  
  STATIC_ASSERT((
    std::is_same< type,
      typename Types::type::template apply<Template>::type
    >::value
  ));
}

#include "../../etude/types/get_type_or.hpp"

template< template<class...> class Template, class Types >
void check_not_defined()
{
  class X {};
  
  typedef typename etude::get_type_or<
    etude::apply_types<Template, Types>, X
  >::type type;
  
  STATIC_ASSERT(( std::is_same<type, X>::value ));
}

#include <tuple>
#include <string>
#include <memory>
#include "../../etude/types/tuple_types.hpp"

int main()
{
  // 通常の場合
  check< std::tuple, etude::types<int, double>, std::tuple<int, double> >();
  // 可変引数じゃないテンプレートの場合
  check< std::shared_ptr, etude::types<int>, std::shared_ptr<int> >();
  // デフォルトテンプレート引数があるテンプレートの場合
  check< std::basic_string, etude::types<char>, std::string >();

  // ::type をとらないといけない場合
  typedef std::tuple<int, int&, char const*> tuple_type;
  check< std::tuple, etude::tuple_types<tuple_type>, tuple_type >();
  typedef std::pair<int*, int&&> pair_type;
  check< std::pair, etude::tuple_types<pair_type>, pair_type >();
  
  // 定義されない場合
  check_not_defined< std::tuple, std::tuple<int, char> >();
}
