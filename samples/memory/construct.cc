//
//  construct.hpp の 実用例
// 
//    特に難しいところはないはず。
//    基本的にライブラリの中で使うもんなので、効率的には使わないほうがいいかも。
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#include "../../etude/memory/construct.hpp"

#include <iostream>
#include "../../etude/immovable.hpp"

struct person
  : etude::immovable<>
{
  std::string name;
  int age;
  
  person( std::string const& name_, int age_ )
    : name(name_), age(age_) {}
    
  person( std::string && name_, int age_ )
    : name( std::move(name_) ), age(age_) {}
  
  ~person() {
    std::cout << name << " (" << age << "): Bye.\n";
  }
  
  void foo()
  {
    std::cout << name << " (" << age << "): Foo!\n";
  }
  
};


#include <memory>
#include "../../etude/memory/operator_new.hpp"
#include "../../etude/memory/storage.hpp"
#include "../../etude/in_place.hpp"

int main()
{
  etude::storage<person> buf;
  
  {
    auto p = etude::construct<person>( buf.address(), "SubaruG", 25 );
    p->foo();
  }
  {
    auto p = etude::construct<person>( etude::in_place( "A", 18 ), buf.address() );
    p->foo();
  }
  {
    auto p = etude::construct( etude::in_place<person>( "B", 15 ), buf.address() );
    p->foo();
  }
  
  {
    auto p0 = etude::operator_new( sizeof(person) );
    auto p1 = etude::construct<person>( std::move(p0), "C", 17 );
    p1->foo();
  }
  
  {
    auto p0 = etude::operator_new<person>();
    std::unique_ptr<person> p1 = etude::construct<person>( etude::in_place( "D", 17 ),
      std::move(p0) );
    p1->foo();
  }
  {
    auto p0 = etude::operator_new<person>();
    std::unique_ptr<person> p1 = etude::construct( etude::in_place<person>( "E", 16 ),
      std::move(p0) );
    p1->foo();
  }
}
