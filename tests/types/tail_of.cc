//
//  tail_of のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/types/tail_of.hpp"

#include <type_traits>

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

// チェック用関数
template< class Checked, class Expected >
void check()
{
  STATIC_ASSERT((
    std::is_same<
      typename etude::tail_of<Checked>::type, Expected
    >::value
  ));
}

#include "../../etude/types/get_type_or.hpp"

template< class Checked >
void check_not_defined()
{
  class X {};
  
  STATIC_ASSERT((
    std::is_same< X,
      typename etude::get_type_or<etude::tail_of<Checked>, X>::type
    >::value
  ));
}

template<class... Ts>
struct const_types
{
  typedef etude::types<Ts const...> type;
};

int main()
{
  check_not_defined< etude::types<> >();
  check< etude::types<int>, etude::types<> >();
  check< etude::types<double, int>, etude::types<int> >();
  check< etude::types<char, double, int>, etude::types<double, int> >();
  
  // 一回 type を取る必要がある場合
  check< const_types<char, double, int&>, etude::types<double const, int&> >();
  check_not_defined< const_types<> >();
  
  // 無関係な型
  class X{};
  check_not_defined< X >();
}
