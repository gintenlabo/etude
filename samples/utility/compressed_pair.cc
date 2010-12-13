//
//  compressed_pair.hpp の 実用例
// 
//    っていうか基本的に Boost と同じなわけですが、
//    わずかに違う move 周りと piecewise-constrution 周りを。
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#include "../../etude/utility/compressed_pair.hpp"

#include <iostream>
#include "../../etude/noncopyable.hpp"

struct person
  : etude::noncopyable
{
  std::string name;
  int age;
  
  person( std::string const& name_, int age_ )
    : name(name_), age(age_) {}
    
  person( std::string && name_, int age_ )
    : name( std::move(name_) ), age(age_) {}

  void foo()
  {
    std::cout << name << " (" << age << "): Foo!\n";
  }
  
};

class empty_class {};

#include <tuple>
#include <memory>
#include "../../etude/scoped.hpp"

int main()
{
  // move する
  etude::compressed_pair< empty_class, std::unique_ptr<person> >
    p1( empty_class(), etude::scoped( new person( "A", 25 ) ) );
  
  // Foo!
  p1.second()->foo();
  // ちゃんと compress 出来てることを確認
  std::cout << sizeof(std::unique_ptr<person>) << std::endl;
  std::cout << sizeof(p1) << std::endl;
  
  // 直接構築する
  etude::compressed_pair<empty_class, person>
    p2( etude::piecewise_construct, std::make_tuple(), std::make_tuple( "B", 19 ) );
  
  // Foo!
  p2.second().foo();
  // 同じく、ちゃんと compress 出来てることを確認
  std::cout << sizeof(person) << std::endl;
  std::cout << sizeof(p2) << std::endl;
}
