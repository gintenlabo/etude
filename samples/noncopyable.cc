//
//  noncopyable の使用例
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#include "../etude/noncopyable.hpp"

#include <vector>
#include <iostream>

// 使用法１： 構造体の基底クラスとして
struct Hoge
  : etude::noncopyable<>
{
  // gcc 4.5.0 には implicit move がないので
  // きちんと = default; する
  Hoge( Hoge&& ) = default;
  
  explicit Hoge( int n ) {
    vec.reserve( n );
    for( int i = 0; i < n; ++i ) {
      vec.push_back( i );
    }
  }
  
  std::vector<int> vec;
  
};

// 使用法２： noncopyable なラッパクラスとして使う
etude::noncopyable<std::vector<int>> f( int n )
{
  etude::noncopyable<std::vector<int>> vec( 10 ); // コンストラクタに引数が渡せる
  
  // テンプレート引数を継承するので、その型と同じように扱える
  for( int i = 0; i < n; ++i ) {
    vec[i] = i;
  }
  
  return vec;
}


int main()
{
  auto x = Hoge( 10 );
  auto y = f( 10 );
  
  std::cout << std::boolalpha;
  std::cout << ( x.vec == y ) << std::endl;
  
}
