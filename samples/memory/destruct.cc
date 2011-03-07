//
//  destruct.hpp の 実用例
// 
//    何をやってるかさえわかれば、特に難しいところはないはず。
//    フリーダムなメモリ管理に絡むエラーを、少しでも少なくするためのものです。
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#include "../../etude/memory/destruct.hpp"

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
#include <boost/assert.hpp>
#include "../../etude/memory/operator_new.hpp"
#include "../../etude/memory/storage.hpp"
#include "../../etude/in_place.hpp"
#include "../../etude/make_unique.hpp"

int main()
{
  {
    // 普通に構築
    auto p = etude::make_unique<person>( "A", 20 );
    p->foo();
    
    // オブジェクトを破棄して抜け殻のメモリ領域を取得
    auto vp = etude::destruct( std::move(p) );
    BOOST_ASSERT( !p && vp );
    
    // 抜け殻に再度オブジェクトを構築
    p = etude::construct<person>( std::move(vp), "A'", 21 );
    BOOST_ASSERT( p && !vp );
    p->foo();
    
    // unique_ptr なので抜け殻を受けとならなくても大丈夫
    etude::destruct( std::move(p) );
    BOOST_ASSERT( !p && !vp );
  }
  
  {
    // etude::construct で作ったオブジェクトの場合はー。
    auto vp = etude::operator_new( sizeof(person) ); 
    // または etude::operator_new<person>();
    auto p = etude::construct<person>( std::move(vp), "B", 20 );
    BOOST_ASSERT( p && !vp );
    p->foo();
    
    // オブジェクトを破棄して抜け殻を取得
    vp = etude::destruct( std::move(p) );
    BOOST_ASSERT( !p && vp );
    
    // 抜け殻に再度構築
    p = etude::construct<person>( std::move(vp), "B'", 19 );
    BOOST_ASSERT( p && !vp );
    p->foo();
  }
}
