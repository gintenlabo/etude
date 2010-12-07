//
//  indices.hpp の 実用例
// 
//    要するにこんなことが出来ます。
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
//  参考： http://www.rhinocerus.net/forum/language-c-moderated/
//    439657-c-0x-tuple-unpacking-arguments-function-call.html#post2038703
//
#include "../etude/indices.hpp"

#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>

// std::unique_ptr<T> と書くのが面倒なので書いたラップ関数
// 今回の本題とは特に関係ないけど、便利ですよ。
template< class T, class D = std::default_delete<T> >
inline std::unique_ptr<T, D> to_unique( T* p, D d = D() ) {
  return std::unique_ptr<T, D>( p, std::forward<D>(d) );
}

// 本題

// 簡単に言うと、 std::tuple の unpack が出来るようになる
// 例として unpack してコンストラクタに渡してみる

// 実装用
template<class T, class Tuple, std::size_t... Indices>
inline std::unique_ptr<T> make_unique_from_tuple_( Tuple && t, etude::indices<Indices...> ) {
  (void)t;  // 空 tuple の場合の Warning 避け
  return to_unique( new T( std::get<Indices>( std::forward<Tuple>(t) )... ) );
}
// 本体
template<class T, class Tuple>
inline std::unique_ptr<T> make_unique_from_tuple( Tuple && t ) {
  return make_unique_from_tuple_<T>( std::forward<Tuple>(t),
    etude::make_indices<
      std::tuple_size<typename std::remove_reference<Tuple>::type>::value
    >()
  );
}

// 実際に試してみる

struct hoge
{
  hoge()
    : x(), y() {}
  
  hoge( int x_, int y_ )
    : x(x_), y(y_) {}

  int x, y;
};

#include <iostream>

int main()
{
  // 空の tuple からの実体化
  auto p = make_unique_from_tuple<hoge>( std::make_tuple() );
  std::cout << p->x << " " << p->y << std::endl;  // 0 0
  
  // 引数付き
  p = make_unique_from_tuple<hoge>( std::make_tuple( 1, 2 ) );
  std::cout << p->x << " " << p->y << std::endl;  // 1 2
}
