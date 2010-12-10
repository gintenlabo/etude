//
//  etude/memory/pseudo_destructor_call.hpp に対するテスト
//    というより仕様
//
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#include "../../etude/memory/pseudo_destructor_call.hpp"

#include <type_traits>
#include <utility>
#include <memory>
#include <boost/assert.hpp>

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

template<class T>
void check()
{
  typedef etude::pseudo_destructor_call<T> pseudo_destructor_call;
  
  // 空で、かつ trivial
  STATIC_ASSERT(( std::is_empty<pseudo_destructor_call>::value ));
  STATIC_ASSERT(( std::is_trivial<pseudo_destructor_call>::value ));
  
  // 戻り値型チェック
  typedef typename pseudo_destructor_call::result_type result_type;
  pseudo_destructor_call const f = {};
  STATIC_ASSERT(( std::is_same<result_type, void>::value ));
  STATIC_ASSERT(( std::is_same<decltype( f( std::declval<T*>() ) ), void>::value ));
  STATIC_ASSERT(( std::is_same<decltype( f( std::declval<T&>() ) ), void>::value ));
}

#include "test_utilities.hpp"

struct hoge
  : lifetime_check<hoge>
{
  // empty
};

int main()
{
  check<int>(); // 組み込み型
  check<char*>(); // ポインタ
  check<hoge>(); // ユーザ定義型
  
  typedef storage_of<hoge>::type hoge_storage;
  
  hoge_storage buf;
  typedef std::unique_ptr< hoge, etude::pseudo_destructor_call<hoge> > pointer;
  
  // 構築されてないことを確認
  BOOST_ASSERT( !hoge::is_initialized(&buf) );
  
  // 構築
  pointer p( ::new(&buf) hoge() );
  // 構築されたことを確認
  BOOST_ASSERT( hoge::is_initialized(&buf) );
  
  // 破棄
  p.reset();
  // 破棄されたことを確認
  BOOST_ASSERT( !hoge::is_initialized(&buf) );
  
  // 再構築
  p.reset( ::new(&buf) hoge() );
  // 構築されたことを確認
  BOOST_ASSERT( hoge::is_initialized(&buf) );
  
  // 参照でも破棄できることをチェック
  {
    etude::pseudo_destructor_call<hoge> const del = {};
    hoge& x = *p.release();
    del( x );
  }
  // 破棄されたことを確認
  BOOST_ASSERT( !hoge::is_initialized(&buf) );
}
