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
template< class Checked, class Expected >
void check()
{
  STATIC_ASSERT(( std::is_same<typename Checked::type, Expected>::value ));
}

#include "../../etude/types/get_type_or.hpp"

template< class Checked >
void check_not_defined()
{
  class X {};
  
  STATIC_ASSERT((
    std::is_same< X,
      typename etude::get_type_or<Checked, X>::type
    >::value
  ));
}

int main()
{
  check_not_defined< etude::nth_of<0> >();
  check_not_defined< etude::nth_of<100> >();
  
  check< etude::nth_of<0, int>, int >();
  check_not_defined< etude::nth_of<1, int> >();
  check_not_defined< etude::nth_of<1000, int> >();
  
  check< etude::nth_of<0, double, float>, double >();
  check< etude::nth_of<1, double, float>, float >();
  check_not_defined< etude::nth_of<2, double, float> >();
  check_not_defined< etude::nth_of<10000, double, float> >();
  
  check< etude::nth_of<0, int&, double&, char&>, int& >();
  check< etude::nth_of<1, int&, double&, char&>, double& >();
  check< etude::nth_of<2, int&, double&, char&>, char& >();
  check_not_defined< etude::nth_of<3, int&, double&, char&> >();
  check_not_defined< etude::nth_of<100000, int&, double&, char&> >();

  check< etude::nth_of<3, void, int, char, double, int*, void(), float&&>, double >();
  check_not_defined< etude::nth_of<-1, void> >();
}
