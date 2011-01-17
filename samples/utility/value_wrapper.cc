//
//  value_wrapper.hpp の 実用例
// 
//    要するにこんなことが出来ます。
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#include "../../etude/utility/value_wrapper.hpp"
#include "../../etude/noncopyable.hpp"

#include <tuple>
#include <utility>

// 任意のクラスに対し、引数を pack したタプルから one-phase construction できる。
// やってみよう。
#include <iostream>

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

int main()
{
  // 普通に構築
  etude::value_wrapper<person> a_( etude::emplace_construct, "A", 19 );
  auto& a = a_.get();
  a.foo();
  
  // タプルから one-phase construction
  etude::value_wrapper<person> b_( etude::unpack_construct, std::make_tuple( "B", 21 ) );
  auto& b = b_.get();
  b.foo();
  
  // タプル（ lvalue ）でも問題ないよ？
  // もっと言うと、 pair でもいいんだよ？
  std::pair<char const*, int> p( "C", 18 );
  etude::value_wrapper<person> c_( etude::unpack_construct, p );
  auto& c = get(c_);
  c.foo();
  
  // 毎回 get するの面倒？ うん、僕もそう思う。
  // でも、このクラスを直接使うことは僕としては考えてなくて、
  // 今回みたいに & で受けるか、あるいはもっと多い例としては、
  // クラスのメンバとして使うことを想定して書いたんだ。
  // なので、 value_wrapper を使うクラスで、その都度アクセサを書いて欲しい。
  // EBO に対応してる関係上、メンバ変数として公開することも出来ないしね。
  
  
  // もう一つの使い方
  // 再代入や move のできる const クラスとして使う
  etude::value_wrapper<std::string const> s0 = "hoge";
  // 通常は lvalue reference は取得できない
  // std::string& s = s0.get();
  // しかし move することはできる
  etude::value_wrapper<std::string const> s1 = std::move(s0);
  // 再代入も行える
  s0 = "fuga";
  
  std::cout << "s0: " << s0.get() << std::endl;
  std::cout << "s1: " << s1.get() << std::endl;
}
