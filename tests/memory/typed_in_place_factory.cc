//
//  etude/memory/typed_in_place_factory.hpp に対するテスト
//    仕様書も兼ねる感じで詳細に。
//
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#include "../../etude/memory/typed_in_place_factory.hpp"


// #include <boost/test/minimal.hpp>  // gcc 4.5.0 -std=c++ では使用不可（Boost 1.45.0）
#include <boost/assert.hpp>
#include "../../etude/memory/is_typed_in_place_factory.hpp"

// 部品
#include "test_utilities.hpp"
#include <utility>
#include <memory>
#include <functional>

template<class T1, class T2>
bool is_type_of( T2 && ) {
  return std::is_same<T1&&, T2&&>::value;
}

// 基本的なチェック（基本特性 ＋ move 特性）
template<class T, class... Args>
void basic_check( etude::typed_in_place_factory<T, Args...> x )
{
  typedef etude::typed_in_place_factory<T, Args...> in_place_factory_type;
  typedef typename in_place_factory_type::tuple_type tuple_type;
  
  // type traits
  BOOST_ASSERT(( etude::is_typed_in_place_factory<in_place_factory_type>::value ));
  
  BOOST_ASSERT(( std::is_same<std::tuple<Args...>, tuple_type>::value ));
  BOOST_ASSERT(( std::is_convertible<etude::in_place_factory<Args...>, in_place_factory_type>::value ));
  BOOST_ASSERT(( std::is_convertible<tuple_type, in_place_factory_type>::value ));
  
  // observers type check
  BOOST_ASSERT( is_type_of<tuple_type const&>( x.get_tuple() ) );
  BOOST_ASSERT( is_type_of<tuple_type&&>( x.move_tuple() ) );
  BOOST_ASSERT( is_type_of<tuple_type const&>( get_tuple(x) ) );
  BOOST_ASSERT( is_type_of<tuple_type&&>( get_tuple( std::move(x) ) ) );
  
  // appliacation
  typename storage_of<T>::type buf;
  std::unique_ptr<T, pseudo_destructor_call<T>> p( x.move_apply(&buf) );
}

// copy 可能な in_place_factory に対する追加チェック
template<class T, class... Args>
void check_copy_apply( etude::typed_in_place_factory<T, Args...> x )
{
  basic_check( x );
  typedef etude::typed_in_place_factory<T, Args...> in_place_factory_type;
  typedef typename in_place_factory_type::tuple_type tuple_type;
  
  BOOST_ASSERT(( std::is_convertible<etude::in_place_factory<Args...> const&, in_place_factory_type>::value ));
  BOOST_ASSERT(( std::is_convertible<tuple_type const&, in_place_factory_type>::value ));
  
  // appliacations
  typename storage_of<T>::type buf;
  std::unique_ptr<T, pseudo_destructor_call<T>> p;
  
  p.reset( x.apply(&buf) );
  p.reset();
  
  p.reset( etude::apply_typed_in_place( x, &buf ) );
  p.reset();
  
  p.reset( etude::apply_typed_in_place( std::move(x), &buf ) );
  p.reset();
}

struct hoge
  : lifetime_check<hoge>
{
  std::unique_ptr<int> p;
  int x; double y;
  
  hoge( int x_, double y_ )
    : x(x_), y(y_) {}
  
  hoge( std::unique_ptr<int> && p_, int x_, double y_ )
    : p( std::move(p_) ), x(x_), y(y_) {}
  
};

int main()
{
  int x; int const y = 0;
  
  // 束縛される型のチェック
  BOOST_ASSERT(( is_type_of<etude::typed_in_place_factory<hoge>>( etude::in_place<hoge>() ) ));
  // in_place では、一時変数は rvalue-reference として、参照はそのまま束縛される
  BOOST_ASSERT(( is_type_of<etude::typed_in_place_factory<hoge, int&, int const&, int&&>>
                                      ( etude::in_place<hoge>( x, y, 1 ) ) ));
  // in_place_by_ref では、一時変数は値で、参照はそのまま束縛される
  BOOST_ASSERT(( is_type_of<etude::typed_in_place_factory<hoge, int&, int const&, int>>
                               ( etude::in_place_by_ref<hoge>( x, y, 1 ) ) ));
  // in_place_by_val では、一時変数／参照は値で、ref は参照で束縛される
  BOOST_ASSERT(( is_type_of<etude::typed_in_place_factory<hoge, int, int, int, int&, int const&>>
    ( etude::in_place_by_val<hoge>( x, y, 1, std::ref(x), std::cref(x) ) ) ));
  
  // 実際のチェック
  check_copy_apply( etude::in_place<int>() );
  basic_check( etude::in_place<bool>( std::unique_ptr<int>() ) );
  basic_check( etude::in_place<hoge>( 1, 2 ) ); // rvalue-reference はコピー出来ない
  check_copy_apply( etude::in_place_by_val<hoge>( 1, 2 ) ); // コピーする場合は値で
  basic_check( etude::in_place<hoge>( std::unique_ptr<int>(), 3, 4 ) );
  
  // 型変換チェック
  {
    // tuple -> in_place_factory
    // 直接変換もあるが基本的にヘルパ関数を使う
    // テンプレートパラメータが厳密に一致していないと変換できないため
    // そうしている理由は、タプルを引数に in_place する場合の混乱を避けるため
    std::tuple<int, int> t( 5, 6 );
    auto in_place_1 = etude::in_place_from_tuple<hoge>(t);
    auto in_place_2 = etude::in_place_from_tuple<hoge>( std::make_tuple( std::unique_ptr<int>(), 7, 8 ) );
    
    // typed_in_place_factory 間の相互変換
    etude::typed_in_place_factory<hoge, double, float> in_place_3 = in_place_1;
    etude::typed_in_place_factory<hoge, std::unique_ptr<int>, unsigned int, double> in_place_4 = std::move(in_place_2);
    
    // in_place_factory -> typed_in_place_factory
    etude::typed_in_place_factory<hoge, double, float> in_place_5 = etude::in_place( 1, 1 );
    etude::typed_in_place_factory<hoge, std::unique_ptr<int>, unsigned int, double> in_place_6 = etude::in_place( std::unique_ptr<int>(), 1, 1 );
  }
  
  // 型変換が出来ないことを確かめる
  // 関連付けられた型が違う場合
  BOOST_ASSERT(( !std::is_convertible<
    etude::typed_in_place_factory<int>, etude::typed_in_place_factory<double>
  >::value ));
  // 引数の数が違う場合
  BOOST_ASSERT(( !std::is_convertible<
    etude::typed_in_place_factory<hoge>, etude::typed_in_place_factory<hoge, int, int>
  >::value ));
  BOOST_ASSERT(( !std::is_convertible<
    etude::typed_in_place_factory<hoge, int, int>, etude::typed_in_place_factory<hoge>
  >::value ));
  // typed_in_place_factory -> in_place_factory への変換は無理
  BOOST_ASSERT(( !std::is_convertible<
    etude::typed_in_place_factory<hoge, int, int>, etude::in_place_factory<int, int>
  >::value ));
  // バラバラの型
  BOOST_ASSERT(( !std::is_convertible<
    etude::typed_in_place_factory<hoge, int, int>, etude::typed_in_place_factory<hoge, char*, int>
  >::value ));
  // in_place_factory からも無理？
  BOOST_ASSERT(( !std::is_convertible<
    etude::in_place_factory<int, int>, etude::typed_in_place_factory<hoge, char*, int>
  >::value ));
}
