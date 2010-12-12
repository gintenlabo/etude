//
//  simple_wrapper:
//    値に対する単純なラッパークラス
// 
//    ADL ガードを搭載した単純なラッパークラスです。主に EBO をするために使います。
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_UTILITY_INCLUDED_SIMPLE_WRAPPER_HPP_
#define ETUDE_UTILITY_INCLUDED_SIMPLE_WRAPPER_HPP_

#include <type_traits>
#include <utility>

namespace etude {
 namespace simple_wrapper_ { // ADL 回避
 
  template<class T, class = void>
  struct simple_wrapper_
  {
    // 引数転送
    // ここでは enable_if はしない
    template<class... Args>
    explicit simple_wrapper_( Args&&... args )
      : x{ std::forward<Args>(args)... } {}
    
    // コピー／ムーブ
    // gcc4.5.0 には implicit move がないので
    simple_wrapper_( simple_wrapper_ const& ) = default;
    simple_wrapper_( simple_wrapper_ && )     = default;
    
    // 取得
    T&       get()       { return x; }
    T const& get() const { return x; }
    
   private:
    T x;
  };
  
  template<class T>
  struct simple_wrapper_<T,
    typename std::enable_if<std::is_empty<T>::value>::type
  >
    : private T
  {
    // 引数転送
    // ここでは enable_if はしない
    template<class... Args>
    explicit simple_wrapper_( Args&&... args )
      : T{ std::forward<Args>(args)... } {}
    
    // コピー／ムーブ
    // gcc4.5.0 には implicit move が（ｒｙ
    simple_wrapper_( simple_wrapper_ const& ) = default;
    simple_wrapper_( simple_wrapper_ && )     = default;
    
    // 取得
    T&       get()       { return *this; }
    T const& get() const { return *this; }
    
  };
  
  // 本体
  template<class T>
  class simple_wrapper
    : private simple_wrapper_<T>
  {
    typedef simple_wrapper_<T> base;
    struct dummy_ {};
    
   public:
    typedef T element_type;
    
    // 型変換コンストラクタ
    template<class U,
      class = typename std::enable_if<
        std::is_convertible<U, T>::value
      >::type
    >
    simple_wrapper( U && src )
      : base( std::forward<U>(src) ) {}
    
    // 型変換ではない一引数のコンストラクタは explicit に
    template<class U,
      class = typename std::enable_if<
        !std::is_convertible<U, T>::value &&
        std::is_constructible<T, U>::value
      >::type
    >
    explicit simple_wrapper( U && x, dummy_ = dummy_() )
      : base( std::forward<U>(x) ) {}
    
    // 一引数じゃない場合は explicit にはしない
    template<class... Args,
      class = typename std::enable_if<
        sizeof...(Args) != 1 &&
        std::is_constructible<T, Args...>::value
      >::type
    >
    simple_wrapper( Args&&... args )
      : base( std::forward<Args>(args)... ) {}
    
    // コピー／ムーブ
    // gcc4.5.0 には（ｒｙ
    simple_wrapper( simple_wrapper const& ) = default;
    simple_wrapper( simple_wrapper && )     = default;
    
    // get
    using base::get;
    
  };
 
  // 自由関数版 get
  template<class T>
  inline T& get( simple_wrapper<T>& x ) {
    return x.get();
  }
  template<class T>
  inline T const& get( simple_wrapper<T> const& x ) {
    return x.get();
  }
  // move 版
  template<class T>
  inline T&& get( simple_wrapper<T> && x ) {
    return std::forward<T>( x.get() );
  }
 
 }  // namespace simple_wrapper_;
 using namespace simple_wrapper_;
}

#endif  // #ifndef ETUDE_UTILITY_INCLUDED_SIMPLE_WRAPPER_HPP_
