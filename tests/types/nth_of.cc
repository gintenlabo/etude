//
//  nth_of のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/types/nth_of.hpp"

#include <type_traits>

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

// チェック用関数
template< std::size_t N, class Checked, class Expected >
void check()
{
  STATIC_ASSERT((
    std::is_same<
      typename etude::nth_of<N, Checked>::type, Expected
    >::value
  ));
}

#include "../../etude/types/get_type_or.hpp"

template< std::size_t N, class Checked >
void check_not_defined()
{
  class X {};
  
  STATIC_ASSERT((
    std::is_same< X,
      typename etude::get_type_or<etude::nth_of<N, Checked>, X>::type
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
  check_not_defined< 0, etude::types<> >();
  check_not_defined< 100, etude::types<> >();
  
  check< 0, etude::types<int>, int >();
  check_not_defined< 100, etude::types<int> >();
  check_not_defined< 100, etude::types<int> >();
  
  check< 0, etude::types<double, float>, double >();
  check< 1, etude::types<double, float>, float >();
  check_not_defined< 2, etude::types<double, float> >();
  check_not_defined< 10000, etude::types<double, float> >();
  
  check< 0, etude::types<int&, double&, char&>, int& >();
  check< 1, etude::types<int&, double&, char&>, double& >();
  check< 2, etude::types<int&, double&, char&>, char& >();
  check_not_defined< 3, etude::types<int&, double&, char&> >();
  check_not_defined< 100000, etude::types<int&, double&, char&> >();

  check< 3, etude::types<void, int, char, double, int*, void(), float&&>, double >();
  check_not_defined< -1, etude::types<void> >();
  
  // 一回 type を取る必要がある場合
  check< 1, const_types<char, double, int>, double const >();
  check_not_defined< 0, const_types<> >();
  
  // 無関係な型
  class X{};
  check_not_defined< 0, X >();
}
