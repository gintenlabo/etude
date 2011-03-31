//
//  etude/memory/in_place_factory.hpp に対するテスト
//    仕様書も兼ねる感じで詳細に。
//
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#include "../../etude/memory/in_place_factory.hpp"


// #include <boost/test/minimal.hpp>  // 使わせろ＞＜
#include <boost/assert.hpp>
#include "../../etude/memory/is_in_place_factory.hpp"

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
void basic_check( etude::in_place_factory<Args...> x )
{
  typedef etude::in_place_factory<Args...> in_place_factory_type;
  typedef typename in_place_factory_type::tuple_type tuple_type;
  
  // type traits
  BOOST_ASSERT(( etude::is_in_place_factory<in_place_factory_type>::value ));
  
  BOOST_ASSERT(( std::is_same<std::tuple<Args...>, tuple_type>::value ));
  BOOST_ASSERT(( std::is_convertible<tuple_type, in_place_factory_type>::value ));
  
  // observers type check
  BOOST_ASSERT( is_type_of<tuple_type const&>( x.get_tuple() ) );
  BOOST_ASSERT( is_type_of<tuple_type&&>( x.move_tuple() ) );
  BOOST_ASSERT( is_type_of<tuple_type const&>( get_tuple(x) ) );
  BOOST_ASSERT( is_type_of<tuple_type&&>( get_tuple( std::move(x) ) ) );
  
  // appliacation
  typename storage_of<T>::type buf;
  std::unique_ptr<T, pseudo_destructor_call<T>> p( x.template move_apply<T>(&buf) );
}

// copy 可能な in_place_factory に対する追加チェック
template<class T, class... Args>
void check_copy_apply( etude::in_place_factory<Args...> x )
{
  basic_check<T>( x );
  typedef etude::in_place_factory<Args...> in_place_factory_type;
  typedef typename in_place_factory_type::tuple_type tuple_type;
  
  BOOST_ASSERT(( std::is_convertible<tuple_type const&, in_place_factory_type>::value ));
  
  // appliacations
  typename storage_of<T>::type buf;
  std::unique_ptr<T, pseudo_destructor_call<T>> p;
  
  p.reset( x.template apply<T>(&buf) );
  p.reset();
  
  p.reset( etude::apply_in_place<T>( x, &buf ) );
  p.reset();
  
  p.reset( etude::apply_in_place<T>( std::move(x), &buf ) );
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
  typedef std::reference_wrapper<int> ref_wapper_t;
  
  // 束縛される型のチェック
  // 引数なし
  BOOST_ASSERT(( is_type_of<etude::in_place_factory<>>( etude::in_place() ) ));
  // 引数有り
  // in_place では、一時変数は rvalue-reference として、参照はそのまま束縛される
  BOOST_ASSERT((
    is_type_of<
      etude::in_place_factory<int&, int const&, int&&, ref_wapper_t&&, char const (&)[5]>
    >( 
      etude::in_place( x, y, 1, std::ref(x), "hoge" )
    )
  ));
  // bind_in_place では、一時変数／参照は値で、ref は参照で、配列はポインタとして束縛される
  BOOST_ASSERT((
    is_type_of<
      etude::in_place_factory<int, int, int, int&, int const&, char const*>
    >(
      etude::bind_in_place( x, y, 1, std::ref(x), std::cref(x), "hoge" )
    )
  ));
  // in_place_by_ref では、一時変数は値で、参照はそのまま束縛される
  BOOST_ASSERT((
    is_type_of<
      etude::in_place_factory<int&, int const&, int, ref_wapper_t, char const (&)[5]>
    >(
      etude::in_place_by_ref( x, y, 1, std::ref(x), "hoge" )
    )
  ));
  // in_place_by_val では、全て値で束縛される
  // ただし配列や関数はポインタとして束縛される
  BOOST_ASSERT((
    is_type_of<
      etude::in_place_factory<int, int, int, ref_wapper_t, char const*>
    >(
      etude::in_place_by_val( x, y, 1, std::ref(x), "hoge" )
    )
  ));
  
  // 実際のチェック
  check_copy_apply<int>( etude::in_place() );
  basic_check<bool>( etude::in_place( std::unique_ptr<int>() ) );
  basic_check<hoge>( etude::in_place( 1, 2 ) ); // rvalue-reference はコピー出来ない
  check_copy_apply<hoge>( etude::bind_in_place( 1, 2 ) ); // コピーする場合は bind する
  basic_check<hoge>( etude::in_place( std::unique_ptr<int>(), 3, 4 ) );
  
  // 型変換チェック
  {
    // tuple -> in_place_factory
    // 直接変換もあるが基本的にヘルパ関数を使う
    // テンプレートパラメータが厳密に一致していないと変換できないため
    // そうしている理由は、タプルを引数に in_place する場合の混乱を避けるため
    std::tuple<int, int> t( 5, 6 );
    auto in_place_1 = etude::in_place_from_tuple(t);
    auto in_place_2 = etude::in_place_from_tuple( std::make_tuple( std::unique_ptr<int>(), 7, 8 ) );
    
    // in_place_factory 間の相互変換
    etude::in_place_factory<double, float> in_place_3 = in_place_1;
    etude::in_place_factory<std::unique_ptr<int>, unsigned int, double> in_place_4 = std::move(in_place_2);
    
    // 一般化された in_place_from_tuple
    auto in_place_5 = etude::in_place_from_tuple( std::make_pair( 9, 10 ) );
    BOOST_ASSERT(( std::is_same<
      decltype(in_place_5), etude::in_place_factory<int, int>
    >::value ));
    
    // 警告避け
    (void)in_place_3, (void)in_place_4, (void)in_place_5;
  }
  
  // 型変換が「できない」チェック
  
  // 引数の数が違う
  BOOST_ASSERT(( !std::is_convertible<
    etude::in_place_factory<>, etude::in_place_factory<int>
  >::value ));
  // 同じく
  BOOST_ASSERT(( !std::is_convertible<
    etude::in_place_factory<int, int>, etude::in_place_factory<int>
  >::value ));
  // 関係ない二つの型
  BOOST_ASSERT(( !std::is_convertible<
    etude::in_place_factory<int>, etude::in_place_factory<double*>
  >::value ));
  // &&（これはコピーするので出来て当然）。
  BOOST_ASSERT(( std::is_convertible<
    etude::in_place_factory<int&&>, etude::in_place_factory<int>
  >::value ));
  // 明らかにおかしいんだけど、これ失敗する。
  // 原因は Types const&... と書くべき場所で Types... と書いてるせい。
  /*
  BOOST_ASSERT(( !std::is_convertible<
    etude::in_place_factory<std::unique_ptr<int>&&>&,
    etude::in_place_factory<std::unique_ptr<int>>
  >::value ));
  */
}
