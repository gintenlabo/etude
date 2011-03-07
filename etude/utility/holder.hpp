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
#include "../types/is_assignable.hpp"
#include "uninitialized.hpp"

namespace etude {
 namespace holder_ { // ADL 回避
 
  template<class T, class Tag, class = void>
  struct holder_
  {
    // 引数転送
    // ここでは enable_if はしない
    template<class... Args>
    explicit holder_( Args&&... args )
      : x( std::forward<Args>(args)... ) {}
    
    // コピー／ムーブ
    // gcc4.5.0 には implicit move がないので
    holder_( holder_ const& ) = default;
    holder_( holder_ && )     = default;
    
    // 取得
    T &      get()       { return x; }
    T const& get() const { return x; }
    T &&    move()       { return std::forward<T>(x); }
    
    // swap
    void swap( holder_& other ) {
      using std::swap;
      swap( x, other.x );
    }
    
   private:
    T x;
  };
  
  // scalar type の場合
  template<class T, class Tag>
  struct holder_< T, Tag,
    typename std::enable_if<std::is_scalar<T>::value>::type
  >
  {
    // デフォルト構築時はきっちり初期化させる
    holder_()
      : x() {}
    // 未初期化にしたい場合は uninitialized を渡す
    holder_( etude::uninitialized_t ) {}
    
    // scalar の場合は複数引数 ctor は無い
    holder_( T x_ )
      : x( x_ ) {}
    
    // 取得
    T &      get()       { return x; }
    T const& get() const { return x; }
    T &&    move()       { return std::forward<T>(x); }
    
    // swap
    void swap( holder_& other ) {
      using std::swap;
      swap( x, other.x );
    }
    
   private:
    T x;
  };
  
  // 空クラスの場合
  template<class T, class Tag>
  struct holder_< T, Tag,
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
    holder_() : T() {}
    
    // コピー／ムーブ
    // gcc4.5.0 には implicit move が（ｒｙ
    holder_( holder_ const& ) = default;
    holder_( holder_ && )     = default;
    
    // 取得
    T&       get()       { return *this; }
    T const& get() const { return *this; }
    T &&    move()       { return std::forward<T>(*this); }
    
    // swap
    void swap( holder_& other ) {
      using std::swap;
      swap( get(), other.get() );
    }
    
  };
  
  // 参照の場合
  template<class T, class Tag>
  struct holder_< T, Tag,
    typename std::enable_if<std::is_reference<T>::value>::type
  >
  {
    // 初期化は T && からのみ
    holder_( T && x )
      : p( boost::addressof(x) ) {}
    
    // 取得
    T&       get()       { return *p; }
    T const& get() const { return *p; }
    T &&    move()       { return std::forward<T>(*p); }
    
    // swap
    void swap( holder_& other ) {
      std::swap( p, other.p );
    }
    
   private:
    typedef typename std::add_pointer<T>::type pointer;
    pointer p;
    
  };
  
  // 本体
  template<class T, class Tag = void>
  class holder
    : private holder_<typename std::remove_const<T>::type, Tag>
  {
    typedef typename std::remove_const<T>::type T_;
    typedef holder_<T_, Tag> base;
    
   public:
    typedef T element_type;
    
    // T_ から構築（ 0 や {} といったものに対処するため）
    template< class U = T_&&,
      class = typename std::enable_if<
        std::is_constructible<T, U>::value
      >::type
    >
    holder( T_ && src )
      : base( std::forward<T_>(src) ) {}
    // 型変換コンストラクタ（ T const& も含む）
    template<class U,
      class = typename std::enable_if<
        std::is_convertible<U, T>::value ||
        ( std::is_scalar<T>::value &&
          std::is_same<typename std::decay<U>::type, etude::uninitialized_t>::value )
      >::type
    >
    holder( U && src )
      : base( std::forward<U>(src) ) {}
    
    // 型変換ではない一引数のコンストラクタは explicit に
    template<class U, class... Args,
      class = typename std::enable_if<
        ( sizeof...(Args) == 0 ) &&
        etude::is_strict_explicitly_convertible<U, T>::value
      >::type
    >
    explicit holder( U && x, Args... )
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
    template<class U, class Tag_,
      class = typename std::enable_if<
        std::is_convertible<U, T_>::value
      >::type
    >
    holder( holder<U, Tag_> const& src )
      : base( src.get() ) {}
    // move
    template<class U, class Tag_,
      class U_ = typename std::remove_const<U>::type,
      class = typename std::enable_if<
        std::is_convertible<U_, T_>::value
      >::type
    >
    holder( holder<U, Tag_> && src )
      : base( src.move() ) {}
    
    // 再代入
    // 単純再代入
    template< class U = T_,
      class = typename std::enable_if<
        etude::is_assignable<T_, U>::value
      >::type
    >
    holder& operator=( T_ && x ) {
      base::get() = std::forward<T_>(x);
      return *this;
    }
    template< class U,
      class = typename std::enable_if<
        etude::is_assignable<T_, U>::value
      >::type
    >
    holder& operator=( U && x ) {
      base::get() = std::forward<U>(x);
      return *this;
    }
    
    // copy
    template<class U, class Tag_,
      class = typename std::enable_if<
        etude::is_assignable<T_, U const&>::value
      >::type
    >
    holder& operator=( holder<U, Tag_> const& rhs ) {
      base::get() = rhs.get();
      return *this;
    }
    // move
    template<class U, class Tag_,
      class U_ = typename std::remove_const<U>::type,
      class = typename std::enable_if<
        etude::is_assignable<T_, U_>::value
      >::type
    >
    holder& operator=( holder<U, Tag_> && rhs ) {
      base::get() = rhs.move();
      return *this;
    }
    
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
    
    // swap
    void swap( holder& x ) {
      base::swap( static_cast<base&>(x) );
    }
    
  };
 
  // 自由関数版 get
  template<class T, class Tag>
  inline T& get( holder<T, Tag>& x ) {
    return x.get();
  }
  template<class T, class Tag>
  inline T const& get( holder<T, Tag> const& x ) {
    return x.get();
  }
  // move 版
  template<class T, class Tag>
  inline typename std::remove_const<T>::type && get( holder<T, Tag> && x ) {
    return x.move();
  }
  
  // 自由関数版 swap
  template<class T, class Tag>
  inline void swap( holder<T, Tag>& one, holder<T, Tag>& another ) {
    one.swap( another );
  }
 
 }  // namespace holder_;
 using namespace holder_;
}

#endif  // #ifndef ETUDE_UTILITY_INCLUDED_SIMPLE_WRAPPER_HPP_
