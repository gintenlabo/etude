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

#include <boost/utility/typed_in_place_factory.hpp>

#include "in_place_factory.hpp"
#include "is_typed_in_place_factory.hpp"

#include <utility>
#include <new>
#include <type_traits>

namespace etude {

  // class template typed_in_place_factory
  
  // Variadic Templates を使って書かれた InPlaceFactory
  // rvalue-reference にも対応している
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
  
  // get_tuple の自由関数版
  // こちらのほうが名前が統一されてるので、基本的にこっちを使うべき
  template<class T, class... Args>
  inline auto get_tuple( typed_in_place_factory<T, Args...> const& x )
    -> decltype( x.get_tuple() ) { return x.get_tuple(); }
  template<class T, class... Args>
  inline auto get_tuple( typed_in_place_factory<T, Args...> && x )
    -> decltype( x.move_tuple() ) { return x.move_tuple(); }
  
  // apply の自由関数版
  // 参照
  template<class T, class... Args>
  inline T* apply_typed_in_place( typed_in_place_factory<T, Args...>& x, void* addr ) {
    return x.apply( addr );
  }
  // const 参照
  template<class T, class... Args>
  inline T* apply_typed_in_place( typed_in_place_factory<T, Args...> const& x, void* addr ) {
    return x.apply( addr );
  }
  // 右辺値参照
  template<class T, class... Args>
  inline T* apply_typed_in_place( typed_in_place_factory<T, Args...> && x, void* addr ) {
    return x.move_apply( addr );
  }
  
  
  // function template in_place
  
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
  

}

#endif  // #ifndef ETUDE_MEMORY_INCLUDED_TYPED_IN_PLACE_FACTORY_
