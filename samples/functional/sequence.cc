//
//  functional/sequence.hpp の使用例
// 
//    …つくっといてなんだけど、普通はあんまり役立たないかも。
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#include "../../etude/functional/sequence.hpp"

#include <iostream>

int main()
{
  auto const f1 = etude::sequence(
    [](){ std::cout << "Hello, "; },
    [](){ std::cout << "sequenced function!\n"; }
  );
  f1();
  
  // 引数を与えます。
  struct Dog {
    void operator()( int n ) {
      while( n --> 0 )
        std::cout << "BowWow\n";
    }
  };
  struct Cat {
    void operator()( int n ) {
      while( n --> 0 )
        std::cout << "Mewmew\n";
    }
  };
  struct Chicken {
    void operator()( int n ) {
      while( n --> 0 )
        std::cout << "cock-a-doodle-doo\n";
    }
  };
  
  auto f2 = etude::sequence( Dog(), Cat(), Chicken() );
  f2(2);
}
