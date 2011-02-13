//
//  holder:
//    値に対する単純なラッパークラス
// 
//    ADL ガードを搭載した単純なラッパークラスです。主に EBO をするために使います。
//    
//  Copyright (C) 2010-11  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_UTILITY_INCLUDED_HOLDER_HPP_
#define ETUDE_UTILITY_INCLUDED_HOLDER_HPP_

#include <type_traits>
#include <utility>
#include <boost/utility/addressof.hpp>
#include "../types/is_strict_explicitly_convertible.hpp"

namespace etude {
 namespace holder_ { // ADL 回避
 
  template<class T, class = void>
  struct holder_
  {
    // 引数転送
    // ここでは enable_if はしない
    template<class... Args>
    explicit holder_( Args&&... args )
      : x( std::forward<Args>(args)... ) {}
    
    // デフォルトコンストラクタは上記のコンストラクタに含まれるはずだが、
    // 何故かは知らないが組み込み型の場合に x を初期化してくれないので、明示的に書く。
    holder_()
      : x() {}
    
    // コピー／ムーブ
    // gcc4.5.0 には implicit move がないので
    holder_( holder_ const& ) = default;
    holder_( holder_ && )     = default;
    
    // 取得
    T &      get()       { return x; }
    T const& get() const { return x; }
    T &&    move()       { return std::forward<T>(x); }
    
   private:
    T x;
  };
  
  template<class T>
  struct holder_<T,
    typename std::enable_if<std::is_empty<T>::value>::type
  >
    : private T
  {
    // 引数転送
    // ここでは enable_if はしない
    template<class... Args>
    explicit holder_( Args&&... args )
      : T( std::forward<Args>(args)... ) {}
    
    // 組み込み型は empty ではないが、こちらでも一応明示的に書く。
    // なお {} による初期化リストはエラーになる場合があったので廃止。
    holder_()
      : T() {}
    
    // コピー／ムーブ
    // gcc4.5.0 には implicit move が（ｒｙ
    holder_( holder_ const& ) = default;
    holder_( holder_ && )     = default;
    
    // 取得
    T&       get()       { return *this; }
    T const& get() const { return *this; }
    T &&    move()       { return std::forward<T>(*this); }
    
  };
  
  // 本体
  template<class T>
  class holder
    : private holder_<typename std::remove_const<T>::type>
  {
    typedef typename std::remove_const<T>::type T_;
    typedef holder_<T_> base;
    struct dummy_ {};
    
   public:
    typedef T element_type;
    
    // T から構築（ 0 や {} といったものに対処するため）
    holder( T && src )
      : base( std::forward<T>(src) ) {}
    // 型変換コンストラクタ（ T const& も含む）
    template<class U,
      class = typename std::enable_if<
        std::is_convertible<U, T>::value
      >::type
    >
    holder( U && src )
      : base( std::forward<U>(src) ) {}
    
    // 型変換ではない一引数のコンストラクタは explicit に
    template<class U,
      class = typename std::enable_if<
        etude::is_strict_explicitly_convertible<U, T>::value
      >::type
    >
    explicit holder( U && x, dummy_ = dummy_() )
      : base( std::forward<U>(x) ) {}
    
    // 一引数じゃない場合は explicit にはしない
    template<class... Args,
      class = typename std::enable_if<
        sizeof...(Args) != 1 &&
        std::is_constructible<T, Args...>::value
      >::type
    >
    holder( Args&&... args )
      : base( std::forward<Args>(args)... ) {}
    
    // コピー／ムーブ
    // gcc4.5.0 には（ｒｙ
    holder( holder const& ) = default;
    holder( holder && )     = default;
    
    // 型変換コンストラクタ
    // copy
    template<class U,
      class = typename std::enable_if<
        std::is_convertible<U, T>::value
      >::type
    >
    holder( holder<U> const& src )
      : base( src.get() ) {}
    // move
    template<class U,
      class = typename std::enable_if<
        std::is_convertible<U, T>::value
      >::type
    >
    holder( holder<U> && src )
      : base( src.move() ) {}
    
    
    // get/move
    
    // get は T が const U の場合に対処するため明示的に指定する
    T &      get()       { return base::get(); }
    T const& get() const { return base::get(); }
    // move はそのまま
    using base::move;
    
    // operator*
    friend T &      operator*( holder &      x ){ return x.get(); }
    friend T const& operator*( holder const& x ){ return x.get(); }
    friend T_ &&    operator*( holder &&     x ){ return x.move(); }
    
    // operator->
    typename std::add_pointer<T>::type operator->() {
      return boost::addressof( get() );
    }
    typename std::add_pointer<T const>::type operator->() const {
      return boost::addressof( get() );
    }
    
  };
 
  // 自由関数版 get
  template<class T>
  inline T& get( holder<T>& x ) {
    return x.get();
  }
  template<class T>
  inline T const& get( holder<T> const& x ) {
    return x.get();
  }
  // move 版
  template<class T>
  inline typename std::remove_const<T>::type && get( holder<T> && x ) {
    return x.move();
  }
 
 }  // namespace holder_;
 using namespace holder_;
}

#endif  // #ifndef ETUDE_UTILITY_INCLUDED_SIMPLE_WRAPPER_HPP_
