//
//  scoped のテストです。
//    
//  Copyright (C) 2010-11  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../etude/scoped.hpp"

#include <type_traits>
#include <boost/assert.hpp>

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )
#define IS_SAME_TYPE( expr1, expr2 )  \
  STATIC_ASSERT(( std::is_same<decltype(expr1), decltype(expr2)>::value ))

#include "../etude/immovable.hpp"
#include <boost/checked_delete.hpp>
#include <boost/optional.hpp>

// 継承チェック
struct base {
  virtual ~base(){}
};
struct derived
  : base {};

// 型チェックをこっちにまとめる
void type_check()
{
  // 普通につくるよん
  IS_SAME_TYPE( std::unique_ptr<int>(), etude::scoped( new int() ) );
  // デリータの種類を明示的に指定するよん
  IS_SAME_TYPE( std::unique_ptr<int>(),
    etude::scoped<std::default_delete<int>>( new int() ) );
  
  // デリータを明示的に渡すよん
  IS_SAME_TYPE( std::unique_ptr<int>(),
    etude::scoped( new int(), std::default_delete<int>() ) );
  {
    // 名前付きでも大丈夫なのん？
    typedef std::default_delete<int> deleter;
    deleter d;
    IS_SAME_TYPE( ( std::unique_ptr<int, deleter>() ), etude::scoped( new int(), d ) );
    
    // 参照渡しには ref を使えばいいのん？
    IS_SAME_TYPE( ( std::unique_ptr<int, deleter&>( 0, d ) ),
      etude::scoped( new int(), std::ref(d) ) );
    
    // 特に意味ないけど cref も使えるのん？
    IS_SAME_TYPE( ( std::unique_ptr<int, deleter const&>( 0, d ) ),
      etude::scoped( new int(), std::cref(d) ) );
  }
  
  // 関数渡しても大丈夫なのん？
  STATIC_ASSERT((
    std::is_same<
      std::unique_ptr<int, void(*)(int*)>,
      decltype( etude::scoped( new int(), boost::checked_delete<int> ) )
    >::value
  ));
  
  
  // "pointer" のあるデリータでテストするよー。
  // まず pointer が通常のポインタの場合だよー。
  struct deleter1 {
    typedef base* pointer;
    void operator()( pointer p ) const {
      delete p;
    }
  };
  
  // その場合は普通と同じだよー。
  IS_SAME_TYPE( ( std::unique_ptr<derived, deleter1>() ),
    etude::scoped<deleter1>( new derived() ) );
  IS_SAME_TYPE( ( std::unique_ptr<derived, deleter1>() ),
    etude::scoped( new derived(), deleter1() ) );
  
  // D::pointer が T* 以外の場合は std::unique_ptr<void, D> になるよー。
  // たとえば boost::optional<int> とかねー。
  struct deleter2 {
    typedef boost::optional<int> pointer;
    void operator()( pointer const& ) const {
      // 特に何もしない。
      // 本来は何らかの資源を所有してるだろうから、それを解放する。
    }
  } d2;
  // テストするよー。
  IS_SAME_TYPE( ( std::unique_ptr<void, deleter2>() ),
    etude::scoped<deleter2>( 1 ) ); // いい感じに型変換してくれるよん
  IS_SAME_TYPE( ( std::unique_ptr<void, deleter2>() ),
    etude::scoped( 2, deleter2() ) );
  IS_SAME_TYPE( ( std::unique_ptr<void, deleter2&>( 0, std::ref(d2) ) ),
    etude::scoped( 2, std::ref(d2) ) );
  
}

struct my_deleter
  : etude::immovable<>  // なんとなく immovable
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
  // 基本的なチェック
  type_check();
  
  my_deleter del;
  
  // 俺々デリータを参照渡しして作るよん
  auto p = etude::scoped( new int(), std::ref(del) );
  
  BOOST_ASSERT( p );
  BOOST_ASSERT( &p.get_deleter() == &del );
  
  // 削除！
  BOOST_ASSERT( del.get_count() == 0 );
  p.reset();
  BOOST_ASSERT( del.get_count() == 1 );
}
