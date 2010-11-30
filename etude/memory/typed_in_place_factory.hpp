//
//  typed_in_place_factory:
//    Boost.TypedInPlaceFactory の C++0x 拡張
// 
//    基本的には etude::in_place_factory と同じです。
//    ファクトリ自身に型が付いてるので、オブジェクトのプロキシのように使えます。
//    また、対応する etude::in_place_factory から暗黙変換できるのも特徴。
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_MEMORY_INCLUDED_TYPED_IN_PLACE_FACTORY_
#define ETUDE_MEMORY_INCLUDED_TYPED_IN_PLACE_FACTORY_

#include "in_place_factory.hpp"
#include <boost/utility/typed_in_place_factory.hpp>

#include <utility>
#include <new>
#include <type_traits>


namespace etude {

  using boost::typed_in_place_factory_base;
  
  // ヘルパメタ関数
  
  // typed_in_place_factory か否か
  template<class T>
  struct is_typed_in_place_factory :
    std::is_base_of<
      typed_in_place_factory_base, typename std::remove_reference<T>::type
    >
  {};
  
  // typed_in_place_factory なら、関連付けられてる型を得る
  // そうでなければ type は定義されない
  template<class T, class = void>
  struct typed_in_place_factory_get_type_impl_ {};
  template<class InPlace>
  struct typed_in_place_factory_get_type_impl_<InPlace,
    typename std::enable_if<is_typed_in_place_factory<InPlace>::value>::type
  >{
    typedef typename InPlace::value_type type;
  };
  template<class T>
  struct typed_in_place_factory_get_type :
    typed_in_place_factory_get_type_impl_<
      typename std::remove_reference<T>::type
    >
  {};
  
  template<class T, class... Args>
  class typed_in_place_factory
    : public typed_in_place_factory_base
  {
    typedef in_place_factory<Args...> impl_t;
    impl_t impl_;
    
    template<class U, class... As>
    friend class typed_in_place_factory;
    
   public:
    typedef T value_type;
    typedef typename impl_t::tuple_type tuple_type;
    
    // 構築
    explicit typed_in_place_factory( Args&& ...args )
      : impl_( std::forward<Args>(args)... ) {}
    
    // タプル、 in_place_factory からの構築
    template<class U,
      class = typename std::enable_if<std::is_convertible<U, impl_t>::value>::type >
    typed_in_place_factory( U && x )
      : impl_( std::forward<U>(x) ) {}
    
    // 他の typed_in_place_factory からの構築
    // 構築するオブジェクトの型は同じじゃないと意味論的におかしい
    template<class... As>
    typed_in_place_factory( typed_in_place_factory<T, As...> const& x )
      : impl_( x.impl_ ) {}
    template<class... As>
    typed_in_place_factory( typed_in_place_factory<T, As...> && x )
      : impl_( std::move(x.impl_) ) {}
    
    // オブジェクト構築
    T* apply( void* addr ) const {
      return impl_.template apply<T>(addr);
    }
    T* move_apply( void* addr ) {
      return impl_.template move_apply<T>(addr);
    }
    
    // 情報取得
    tuple_type const& get_tuple() const { return impl_.get_tuple(); }
    tuple_type&& move_tuple() { return impl_.move_tuple(); }
    
  };
  
  
  // helper functions
  
  // 一時オブジェクトを rvalue-reference として束縛
  // auto を使って束縛されると危険だが、 std::unique_ptr 等に重宝する
  template<class T, class... Args>
  inline typed_in_place_factory<T, Args&&...> in_place( Args&& ...args ) {
    return typed_in_place_factory<T, Args&&...>( std::forward<Args&&>(args)... );
  }
  // 一時オブジェクトは値として、それ以外は参照として束縛する
  // こちらは関数の戻り値として使わない限りは auto で束縛しても問題ない。
  // ただし、使い回す場合は、参照なので不意な変更に注意！
  template<class T, class... Args>
  inline typed_in_place_factory<T, Args...> in_place_by_ref( Args&& ...args ) {
    return typed_in_place_factory<T, Args...>( std::forward<Args>(args)... );
  }
  
  // タプルを in_place_factory に変換する版。
  // とりあえず詰め込んだ値を使ってオブジェクトを構築したい場合に。
  template<class T, class... Args>
  inline typed_in_place_factory<T, Args...>
    in_place_from_tuple( std::tuple<Args...> const& t )
  {
    return t;
  }
  template<class T, class... Args>
  inline typed_in_place_factory<T, Args...>
    in_place_from_tuple( std::tuple<Args...> && t )
  {
    return std::move(t);
  }
  
  // すべて値で束縛する安全版。関数の戻り値としても使える。
  // 参照を束縛したい場合は std::ref を使う。
  template<class T, class... Args>
  inline auto in_place_by_val( Args&& ...args )
    -> decltype( in_place_from_tuple<T>( std::make_tuple( std::forward<Args>(args)... ) ) )
  {
    return in_place_from_tuple<T>( std::make_tuple( std::forward<Args>(args)... ) );
  }
  
  
  // 戻り値と move 対応を盛り込んだヘルパ関数
  
  // boost 対応
  template<class InPlace,
    class T = typename typed_in_place_factory_get_type<InPlace>::type
  >
  inline T* apply_typed_in_place( InPlace const& x, void* addr ) {
    x.apply( addr );
    return static_cast<T*>( addr );
  }
  
  // etude::in_place_factory 版
  // const 参照
  template<class T, class... Args>
  inline T* apply_typed_in_place( typed_in_place_factory<T, Args...> const& x, void* addr ) {
    return x.apply( addr );
  }
  // move
  template<class T, class... Args>
  inline T* apply_typed_in_place( typed_in_place_factory<T, Args...> && x, void* addr ) {
    return x.move_apply( addr );
  }
  
  // apply_in_place<T>( in_place<T>(), addr ) という表現が出来ると嬉しい
  // 型が明確な場合に InPlaceFactory と TypedInPlaceFactory の両方を取るのは単に面倒。
  template<class T, class InPlace,
    class = typename std::enable_if<
      std::is_same< T,
        typename typed_in_place_factory_get_type<InPlace>::type
      >::value
    >::type
  >
  inline T* apply_in_place( InPlace && x, void* addr ) {
    return apply_typed_in_place( std::forward<InPlace>(x), addr );
  }
  
  // 広義の in_place_factory, つまり apply_in_place<T>( x, addr ) と書けるかどうか
  template<class T, class InPlace, class = void>
  struct is_in_place_impl_
    : is_in_place_factory<InPlace> {};
  
  template<class T, class InPlace>
  struct is_in_place_impl_< T, InPlace,
    typename std::enable_if<
      std::is_same< T,
        typename typed_in_place_factory_get_type<InPlace>::type
      >::value
    >::type
  > : std::true_type {};
  
  template<class T, class InPlace>
  struct is_in_place : is_in_place_impl_<T, InPlace> {};
  
  
  // to_tuple の自由関数版
  // こちらのほうが名前が統一されてるので、基本的にこっちを使うべき
  template<class T, class... Args>
  inline auto get_argument_tuple( typed_in_place_factory<T, Args...> const& x )
    -> decltype( x.get_tuple() ) { return x.get_tuple(); }
  template<class T, class... Args>
  inline auto get_argument_tuple( typed_in_place_factory<T, Args...> && x )
    -> decltype( x.move_tuple() ) { return x.move_tuple(); }

}

#endif  // #ifndef ETUDE_MEMORY_INCLUDED_TYPED_IN_PLACE_FACTORY_
