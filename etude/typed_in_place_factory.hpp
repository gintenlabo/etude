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
#ifndef ETUDE_INCLUDED_TYPED_IN_PLACE_FACTORY_
#define ETUDE_INCLUDED_TYPED_IN_PLACE_FACTORY_

#include "in_place_factory.hpp"
#include <boost/utility/typed_in_place_factory.hpp>

#include <utility>
#include <new>
#include <type_traits>


namespace etude {

  using boost::typed_in_place_factory_base;
  
  // ヘルパメタ関数
  template<class T>
  struct is_typed_in_place_factory
    : std::is_base_of<typed_in_place_factory_base, T> {};
  
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
    typedef typename impl_t::arguments arguments;
    
    // 構築
    explicit typed_in_place_factory( Args&& ...args )
      : impl_( std::forward<Args>(args)... ) {}
    
    // タプル、 in_place_factory からの構築
    template<class U,
      class = typename std::enable_if<std::is_convertible<U, impl_t>::value>::type >
    typed_in_place_factory( U && x )
      : impl_( x ) {}
    
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
    arguments const& get_arguments() const { return impl_.get_arguments(); }
    arguments&& move_arguments() { return impl_.move_arguments(); }
    
  };
  
  
  
  // helper function
  
  // 一時オブジェクトを rvalue-reference として束縛
  // auto を使って束縛されると危険
  template<class T, class... Args>
  inline typed_in_place_factory<T, Args&&...> in_place( Args&& ...args ) {
    return typed_in_place_factory<T, Args&&...>( std::forward<Args&&>(args)... );
  }
  // 安全版。しかし明らかに処理は増える
  // std::unique_ptr を使う場合にはこっちは使えません
  template<class T, class... Args>
  inline typed_in_place_factory<T, Args...> in_place_safe( Args&& ...args ) {
    return typed_in_place_factory<T, Args...>( std::forward<Args>(args)... );
  }
  
  
  // 戻り値と move 対応を盛り込んだヘルパ関数
  
  // boost 対応
  template<class InPlace,
    typename T = typename std::enable_if<
      is_typed_in_place_factory<InPlace>::value, typename InPlace::value_type
    >::type
  >
  inline T* apply_in_place( InPlace const& x, void* addr ) {
    x.apply( addr );
    return static_cast<T*>( addr );
  }
  
  // etude::in_place_factory 版
  // const 参照
  template<class T, class... Args>
  inline T* apply_in_place( typed_in_place_factory<T, Args...> const& x, void* addr ) {
    return x.apply( addr );
  }
  // move
  template<class T, class... Args>
  inline T* apply_in_place( typed_in_place_factory<T, Args...> && x, void* addr ) {
    return x.move_apply( addr );
  }

}

#endif  // #ifndef ETUDE_INCLUDED_TYPED_IN_PLACE_FACTORY_
