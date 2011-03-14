//
//  optional_reference:
//    無効値を取るかもしれない参照
// 
//    etude::optional_reference<T> は、 T* に対する簡単なラッパークラスです。
//    boost::optional<T&> を参考にしていますが、微妙にインターフェイスが違うので注意。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_INCLUDED_OPTIONAL_REFERENCE_HPP_
#define ETUDE_INCLUDED_OPTIONAL_REFERENCE_HPP_

#include <utility>
#include <type_traits>
#include <functional> // for std::less

#include <boost/operators.hpp>
#include <boost/none_t.hpp>
#include <boost/assert.hpp>
#include <boost/utility/addressof.hpp>

#include "types/has_common_type.hpp"

namespace etude {

  template<class T>
  class optional_reference
    : boost::totally_ordered< optional_reference<T> >
  {
    typedef optional_reference<T> self_type;
    
   public:
    typedef T    value_type;
    typedef T& element_type;
    typedef T&    reference;
    typedef T*      pointer;
    
    // 無効値
    /*constexpr*/ optional_reference() /*noexcept*/
      : p_() {}
    /*constexpr*/ optional_reference( boost::none_t ) /*noexcept*/
      : p_() {}
    
    // 参照からの構築
    optional_reference( T& x ) /*noexcept*/
      : p_( boost::addressof(x) ) {}
    // cond が true なら x から、そうでなければ無効参照から構築
    optional_reference( bool cond, T& x ) /*noexcept*/
      : p_( cond ? boost::addressof(x) : 0 ) {}
    
    // rvalue からは構築できないよう
    optional_reference( T && x ) = delete;
    optional_reference( bool cond, T && x ) = delete;
    
    
    // 型変換
    template< class U,
      class = typename std::enable_if<
        std::is_convertible<U*, T*>::value
      >::type
    >
    optional_reference( optional_reference<U> const& src ) /*noexcept*/
      : p_( src.get() ) {}
    
    
    // modifiers
    
    // swap
    void swap( self_type& other ) /*noexcept*/ {
      std::swap( p_, other.p_ );
    }
    // 自由関数版
    friend void swap( self_type& one, self_type& another ) /*noexcept*/ {
      one.swap( another );
    }
    
    
    // observers
    
    // get はポインタを返す（ boost::optional とは違う）
    T* get() const /*noexcept*/ { return p_; }
    // 自由関数版
    friend T* get( self_type const& p ) /*noexcept*/ {
      return p.get();
    }
    // 互換性のため get_ptr も用意
    T* get_ptr() const /*noexcept*/ { return p_; }
    // 自由関数版
    friend T* get_pointer( self_type const& x ) /*noexcept*/ {
      return x.get_ptr();
    }
    // 一応参照版も
    T& get_ref() const /*noexcept*/ { 
      BOOST_ASSERT( p_ != 0 );
      return *p_;
    }
    // 参照が欲しい場合は operator* で。
    
    // operator bool
    explicit operator bool() const /*noexcept*/ {
      return p_;
    }
    
    // operator->
    T* operator->() const /*noexcept*/ {
      BOOST_ASSERT( p_ != 0 );
      return p_;
    }
    // operator*
    T& operator*() const /*noexcept*/ {
      BOOST_ASSERT( p_ != 0 );
      return *p_;
    }
    
    // boost::optional の get_value_or 互換
    T& get_value_or( T& default_ ) const /*noexcept*/ {
      return p_ ? *p_ : default_;
    }
    // 遅延評価版
    template< class F,
      class = typename std::enable_if<
        std::is_convertible<typename std::result_of<F()>::type, T&>::value
      >::type
    >
    T& get_value_or_invoke( F && f ) const {
      return p_ ? *p_ : std::forward<F>(f)();
    }
    
    // 比較
    friend bool operator==( self_type const& lhs, self_type const& rhs ) /*noexcept*/ {
      return lhs.p_ == rhs.p_;
    }
    friend bool operator<( self_type const& lhs, self_type const& rhs ) /*noexcept*/ {
      return std::less<T*>()( lhs.p_, rhs.p_ );
    }
    // !=, >, <=, >= は boost::totally_ordered により自動定義される
    
    // 他の型との比較は外で定義する
    
   private:
    T* p_;
    
  };
  
  // 違う型同士の比較
  template<class T, class U,
    class = typename std::enable_if<
      has_common_type<T*, U*>::value
    >::type
  >
  inline bool operator== (
    optional_reference<T> const& lhs, optional_reference<U> const& rhs
  ) /*noexcept*/
  {
    return lhs.get() == rhs.get();
  }
  template<class T, class U,
    class = typename std::enable_if<
      has_common_type<T*, U*>::value
    >::type
  >
  inline bool operator!= (
    optional_reference<T> const& lhs, optional_reference<U> const& rhs
  ) /*noexcept*/
  {
    return !( lhs == rhs );
  }
  // 順序比較も
  template<class T, class U,
    class = typename std::enable_if<
      has_common_type<T*, U*>::value
    >::type
  >
  inline bool operator< (
    optional_reference<T> const& lhs, optional_reference<U> const& rhs
  ) /*noexcept*/
  {
    typedef typename std::common_type<T*, U*>::type pointer;
    return std::less<pointer>()( lhs.get(), rhs.get() );
  }
  template<class T, class U,
    class = typename std::enable_if<
      has_common_type<T*, U*>::value
    >::type
  >
  inline bool operator> (
    optional_reference<T> const& lhs, optional_reference<U> const& rhs
  ) /*noexcept*/
  {
    return rhs < lhs;
  }
  template<class T, class U,
    class = typename std::enable_if<
      has_common_type<T*, U*>::value
    >::type
  >
  inline bool operator<= (
    optional_reference<T> const& lhs, optional_reference<U> const& rhs
  ) /*noexcept*/
  {
    return !( rhs < lhs );
  }
  template<class T, class U,
    class = typename std::enable_if<
      has_common_type<T*, U*>::value
    >::type
  >
  inline bool operator>= (
    optional_reference<T> const& lhs, optional_reference<U> const& rhs
  ) /*noexcept*/
  {
    return !( lhs < rhs );
  }
  
  // 取得
  template<class T>
  inline T& get_optional_value_or( optional_reference<T> const& x, T& default_ ) {
    return x.get_value_or( default_ );
  }
  template<class T, class F,
    class = typename std::enable_if<
      std::is_convertible<typename std::result_of<F()>::type, T&>::value
    >::type
  >
  inline T& get_optional_value_or_invoke( optional_reference<T> const& x, F && f ) {
    return x.get_value_or_invoke( std::forward<F>(f) );
  }
  
  
  // ヘルパ関数
  template<class T>
  inline optional_reference<T> make_optional_reference( T& x ) /*noexcept*/ {
    return optional_reference<T>(x);
  }
  template<class T>
  inline optional_reference<T> make_optional_reference( bool cond, T& x ) /*noexcept*/ {
    return optional_reference<T>( cond, x );
  }
  
  // Nullable Pointer クラスから作る
  template< class P,
    class Result = decltype(
      etude::make_optional_reference( bool( std::declval<P&>() ), *std::declval<P>() )
    )
  >
  inline Result make_optional_reference_if( P && p ) {
    return p ? Result( *std::forward<P>(p) ) : Result();
  }
  
  // lazy 版
  template<class F,
    class Result = decltype(
      etude::make_optional_reference( std::declval<F>()() )
    )
  >
  inline Result make_optional_reference_lazy( bool cond, F && f ) {
    return cond ? Result( std::forward<F>(f)() ) : Result();
  }

} // namespace etude

#endif  // #ifndef ETUDE_INCLUDED_OPTIONAL_REFERENCE_HPP_
