//
//  test_utilities.hpp に対するテスト
//
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#include "test_utilities.hpp"

#include <boost/assert.hpp>
#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

// lifetime_check
#include "../../etude/memory/construct.hpp"
#include <memory>

void test_lifetime_check()
{
  struct tested
    : lifetime_check<tested> {};
  
  // 最初はゼロ。
  BOOST_ASSERT( tested::count_existing_instance() == 0 );
  {
    tested x;
    
    // インスタンスが増えてることを確認
    BOOST_ASSERT( tested::count_existing_instance() == 1 );
    // x がちゃんと登録されていることを確認
    BOOST_ASSERT( tested::is_initialized( &x ) );
    
    // さらにインスタンスを増やす
    tested const y = x;
    
    // インスタンスが増えてることを確認
    BOOST_ASSERT( tested::count_existing_instance() == 2 );
    // y もちゃんと登録されていることを確認
    BOOST_ASSERT( tested::is_initialized( &x ) );
    BOOST_ASSERT( tested::is_initialized( &y ) );
  }
  // 破棄された。
  BOOST_ASSERT( tested::count_existing_instance() == 0 );
  
  {
    // バッファに対して placement new する場合ー。
    etude::storage<tested> buf;
    // この時点ではインスタンスとしてカウントされないし、
    BOOST_ASSERT( tested::count_existing_instance() == 0 );
    // is_initialized も false
    BOOST_ASSERT( !tested::is_initialized( buf.address() ) );
    
    // 構築！
    auto p = etude::construct<tested>( buf.address() );
    BOOST_ASSERT( p.get() == buf.address() );
    // 初期化されたのでインスタンス数は増える
    BOOST_ASSERT( tested::count_existing_instance() == 1 );
    // is_initialized も true になる
    BOOST_ASSERT( tested::is_initialized( buf.address() ) );
    
    // 更に構築してみる
    std::unique_ptr<tested> p2( new tested() );
    void* const p2_ = p2.get();
    // チェックチェック。
    BOOST_ASSERT( tested::count_existing_instance() == 2 );
    BOOST_ASSERT( tested::is_initialized( buf.address() ) );
    BOOST_ASSERT( tested::is_initialized( p2.get() ) );
    
    // p を破棄。
    p.reset();
    BOOST_ASSERT( tested::count_existing_instance() == 1 );
    BOOST_ASSERT( !tested::is_initialized( buf.address() ) );
    BOOST_ASSERT( tested::is_initialized( p2.get() ) );
    
    // p2 も破棄。
    p2.reset();
    BOOST_ASSERT( tested::count_existing_instance() == 0 );
    BOOST_ASSERT( !tested::is_initialized( buf.address() ) );
    BOOST_ASSERT( !tested::is_initialized( p2_ ) );
  }
  
  // 一応もう一回ゼロになってることをチェック
  BOOST_ASSERT( tested::count_existing_instance() == 0 );
}

int main()
{
  test_lifetime_check();
}
