//
//  scoped のテストです。
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../etude/scoped.hpp"

#include <type_traits>
#include <boost/assert.hpp>

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )
#define IS_SAME_TYPE( expr1, expr2 )  \
  STATIC_ASSERT(( std::is_same<decltype(expr1), decltype(expr2)>::value ))

#include "../etude/noncopyable.hpp"
#include <boost/checked_delete.hpp>

struct my_deleter
  : etude::noncopyable  // なんとなく noncopyable
{
  my_deleter()
    : count_(0) {}
  
  // 削除しつつ、こっそり削除したポインタの数を数えるよ！
  template<class T>
  void operator()( T* p ) {
    boost::checked_delete( p );
    ++count_;
  }
  
  // 今まで削除したポインタの数を返すよ！
  int get_count() const { return count_; }
  
 private:
  int count_;
  
};

int main()
{
  my_deleter del;
  
  // 型チェック
  IS_SAME_TYPE( std::unique_ptr<int>(), etude::scoped( new int() ) );
  // デリータを渡しても大丈夫なのん？
  IS_SAME_TYPE( std::unique_ptr<int>(),
    etude::scoped( new int(), std::default_delete<int>() ) );
  {
    // 名前付きでも大丈夫なのん？
    std::default_delete<int> d;
    IS_SAME_TYPE( std::unique_ptr<int>(), etude::scoped( new int(), d ) );
  }
  // 関数渡しても大丈夫なのん？
  IS_SAME_TYPE( ( std::unique_ptr<int, void(*)(int*)>() ),
    etude::scoped( new int(), boost::checked_delete<int> ) );
  
  // デリータを指定して構築するよん
  IS_SAME_TYPE( std::unique_ptr<int>(),
    etude::scoped<std::default_delete<int>>( new int() ) );
  
  // 俺々デリータを参照渡しするよ！
  IS_SAME_TYPE( ( std::unique_ptr<int, my_deleter&>( 0, del ) ),
    etude::scoped( new int(), std::ref(del) ) );
  
  // ちゃんと作れてるん？
  auto p = etude::scoped( new int(), std::ref(del) );
  
  BOOST_ASSERT( p );
  BOOST_ASSERT( &p.get_deleter() == &del );
  
  // 削除！
  BOOST_ASSERT( del.get_count() == 0 );
  p.reset();
  BOOST_ASSERT( del.get_count() == 1 );
}
