//
//  etude/memory/operator_delete.hpp に対するテスト
//
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#include "../../etude/memory/destruct.hpp"
#include "../../etude/memory/operator_new.hpp"
#include "test_utilities.hpp"

#include <boost/assert.hpp>
#include <memory>
#include <type_traits>

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

struct hoge
  : lifetime_check<hoge>
{
  hoge()
    : x() {}
  
  static void* operator new( std::size_t size ) {
    void* const p = ::operator new( size );
    instances_() += 1;
    return p;
  }
  static void operator delete( void* p ) {
    ::operator delete(p);
    instances_() -= 1;
  }
  
  static int count_allocated_instance() {
    return instances_();
  }
  
  int x;
  
 private:
  static int& instances_() {
    static int instances = 0;
    return instances;
  }
  
};

int main()
{
  BOOST_ASSERT( hoge::count_existing_instance() == 0 );
  BOOST_ASSERT( hoge::count_allocated_instance() == 0 );
  
  // std::unique_ptr<T> 版
  {
    std::unique_ptr<hoge> p( new hoge() );
    // ちゃんとメモリが確保され、オブジェクトも構築されてる？
    BOOST_ASSERT( hoge::count_allocated_instance() == 1 );
    BOOST_ASSERT( hoge::count_existing_instance() == 1 );
    
    void* const addr = p.get();
    
    auto vp = etude::destruct( std::move(p) );
    // 型チェック
    STATIC_ASSERT(( std::is_same<decltype(vp),
      std::unique_ptr<void, etude::default_deallocate<hoge>>
    >::value ));
    
    // 所有権が移動し、
    BOOST_ASSERT( !p && vp.get() == addr );
    // オブジェクトは破棄され、
    BOOST_ASSERT( hoge::count_existing_instance() == 0 );
    // でもメモリは解放されてないことを確認。
    BOOST_ASSERT( hoge::count_allocated_instance() == 1 );
    
    // 再構築！
    p = etude::construct<hoge>( std::move(vp) );
    // ちゃんとオブジェクトが構築されてることを確認
    BOOST_ASSERT( hoge::count_allocated_instance() == 1 );
    BOOST_ASSERT( hoge::count_existing_instance() == 1 );
    
    // destruct し、戻り値を受け取らない場合
    etude::destruct( std::move(p) );
    BOOST_ASSERT( !p && !vp );
    
    // 問題なくメモリも解放されてることを確認
    BOOST_ASSERT( hoge::count_existing_instance() == 0 );
    BOOST_ASSERT( hoge::count_allocated_instance() == 0 );
  }
  
  BOOST_ASSERT( hoge::count_existing_instance() == 0 );
  BOOST_ASSERT( hoge::count_allocated_instance() == 0 );
  
  // construct されたオブジェクトを destruct する
  {
    typedef checked_storage<hoge> storage_type;
    
    // メモリ確保
    auto vp = storage_type::allocate();
    void* const addr = vp.get();
    BOOST_ASSERT( storage_type::count_allocated_instance() == 1 );
    BOOST_ASSERT( storage_type::is_allocated( addr ) );
    
    // 構築
    auto p = etude::construct<hoge>( std::move(vp) );
    BOOST_ASSERT( hoge::count_existing_instance() == 1 );
    BOOST_ASSERT( hoge::is_initialized( addr ) );
    
    // 破棄
    vp = etude::destruct( std::move(p) );
    
    // 所有権が移動し、
    BOOST_ASSERT( !p && vp.get() == addr );
    // オブジェクトは破棄され、
    BOOST_ASSERT( hoge::count_existing_instance() == 0 );
    BOOST_ASSERT( !hoge::is_initialized( addr ) );
    // でもメモリは解放されてないことを確認。
    BOOST_ASSERT( storage_type::count_allocated_instance() == 1 );
    BOOST_ASSERT( storage_type::is_allocated( addr ) );
    
    // 再構築！
    p = etude::construct<hoge>( std::move(vp) );
    // ちゃんとオブジェクトが構築されてることを確認
    BOOST_ASSERT( hoge::count_existing_instance() == 1 );
    BOOST_ASSERT( hoge::is_initialized( addr ) );
    BOOST_ASSERT( storage_type::count_allocated_instance() == 1 );
    BOOST_ASSERT( storage_type::is_allocated( addr ) );
    
    // destruct し、戻り値を受け取らない場合
    etude::destruct( std::move(p) );
    BOOST_ASSERT( !p && !vp );
    
    // 問題なくメモリも解放されてることを確認
    BOOST_ASSERT( hoge::count_existing_instance() == 0 );
    BOOST_ASSERT( !hoge::is_initialized( addr ) );
    BOOST_ASSERT( storage_type::count_allocated_instance() == 0 );
    BOOST_ASSERT( !storage_type::is_allocated( addr ) );
  }
  
  BOOST_ASSERT( hoge::count_existing_instance() == 0 );
  BOOST_ASSERT( hoge::count_allocated_instance() == 0 );
}
