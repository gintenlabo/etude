//
//  optional:
//    Boost.Optional の C++0x 拡張
// 
//    Boost.Optional の C++0x 拡張版です。
//    rvalue reference と explicit operator bool 、
//    optional<T const> および各種の SFINAE に対応しています。
//    
//    また、その実装は、現行（ Boost 1.46.1 ）の Boost.Optional に比べて、
//    格納される型がデストラクタを持たない場合や、特に参照の場合において
//    大きく最適化されたものになっています。
//    
//    特に etude::optional<T&> は、「 delete できない安全なポインタ」として
//    ほぼオーバーヘッドなく扱えるクラスになっています。
//    
//    現行の Boost.Optional との差異ですが、
//    まず比較演算子が Boost.Optional のそれに比べて、より効率的に実装されています。
//    特に異なる型の optional 同士は Boost.Optional では比較できませんが、
//    etude::optional では、元の型に対して比較が定義されていれば、きちんと比較できます。
//    
//    また、 T が再代入できない型の場合、 boost::optional<T> も再代入できませんでしたが、
//    etude::optional<T> は再構築による再代入が可能になっています。
//    （再代入できる型なら、再構築ではなく再代入による処理を行います）
//    
//    特に etude::optional<T const> は、再代入が可能になっているだけでなく、
//    move 時には非 const の T&& を得られるようになっています。
//    
//    
//    なおこのクラスは、 T がポインタ以外で各操作が例外を投げない場合、
//    標準ライブラリの NullablePointer Requirement を満たします。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_INCLUDED_OPTIONAL_HPP_
#define ETUDE_INCLUDED_OPTIONAL_HPP_

#include <boost/none.hpp>
#include "detail_/optional_impl_.hpp"

#include <utility>
#include <type_traits>
#include <functional> // for std::less

#include <boost/assert.hpp>
#include <boost/utility/addressof.hpp>

#include "operators/totally_ordered.hpp"
#include "operators/partially_ordered.hpp"

#include "types/is_array_of_unknown_bound.hpp"
#include "types/is_assignable_or_convertible.hpp"
#include "types/pointee.hpp"

#include "types/is_equality_comparable.hpp"
#include "types/is_less_than_comparable.hpp"
#include "types/is_less_or_equal_comparable.hpp"
#include "functional/equal_to.hpp"
#include "functional/less.hpp"
#include "functional/less_equal.hpp"
#include "utility/pointee_equal.hpp"
#include "utility/pointee_before.hpp"
#include "utility/pointee_less_or_equal.hpp"

#include "types/decay_and_strip.hpp"

namespace etude {


  template<class T>
  class optional
    : etude::totally_ordered< optional<T>, boost::none_t,
        etude::partially_ordered< optional<T>, typename std::remove_reference<T>::type >
      >
  {
    static_assert(
      !std::is_void<T>::value && !std::is_function<T>::value &&
      !etude::is_array_of_unknown_bound<T>::value,
      "T must be complete type."
    );
    
    typedef optional<T> self_type;
    typedef typename std::remove_cv<T>::type T_;
    typedef optional_impl_<T_> impl_type;
    impl_type impl_;
    
   public:
    // 型定義
    typedef T element_type;
    typedef typename std::remove_reference<T_>::type value_type;
    
    typedef T&             reference;
    typedef T const& const_reference;
    typedef T_&&    rvalue_reference;
    
    typedef typename std::add_pointer<T>::type             pointer;
    typedef typename std::add_pointer<T const>::type const_pointer;
    
    
    // 無効値から構築
    optional() {}
    optional( boost::none_t ) {}
    
    // T_ からの構築
    template< class U = T_&&,
      class = typename std::enable_if<std::is_constructible<T, U>::value>::type
    >
    optional( T_ && x ) {
      impl_.construct( std::forward<T_>(x) );
    }
    // 型変換構築
    // 参照の場合には、一時オブジェクトへの参照を束縛させないよう無効化。
    template<class U,
      class = typename std::enable_if<
        !std::is_reference<T>::value &&
        std::is_convertible<U, T>::value &&
        !etude::is_in_place_applyable<U, T>::value  // InPlace の場合はそっち優先
      >::type
    >
    optional( U && x ) {
      impl_.construct( std::forward<U>(x) );
    }
    // 更に T が lvalue reference の場合、 rvalue を束縛できないようにする
    template< class U = T,
      class = typename std::enable_if<std::is_lvalue_reference<U>::value>::type
    >
    optional( typename std::remove_reference<U>::type && x ) = delete;
    
    // 条件付き構築
    template< class U = T_&&,
      class = typename std::enable_if<std::is_constructible<T, U>::value>::type
    >
    optional( bool cond, T_ && x ) {
      if( cond ) {
        impl_.construct( std::forward<T_>(x) );
      }
    }
    template<class U,
      class = typename std::enable_if<
        !std::is_reference<T>::value && // 参照の場合は無効化
        std::is_convertible<U, T>::value &&
        !etude::is_in_place_applyable<U, T>::value  // InPlace の場合はそっち優先
      >::type
    >
    optional( bool cond, U && x ) {
      if( cond ) {
        impl_.construct( std::forward<U>(x) );
      }
    }
    // T が lvalue reference の場合
    template< class U = T,
      class = typename std::enable_if<std::is_lvalue_reference<U>::value>::type
    >
    optional( bool cond, typename std::remove_reference<U>::type && x ) = delete;
    
    
    // copy/move は基底クラスのを使う
    
    // 型変換
    template< class U,
      class U_ = typename std::remove_cv<U>::type,
      class = typename std::enable_if<
        std::is_convertible<U_ const&, T_>::value
      >::type
    >
    optional( optional<U> const& src ) {
      if( src ) {
        impl_.construct( *src );
      }
    }
    template< class U,
      class U_ = typename std::remove_cv<U>::type,
      class = typename std::enable_if<
        std::is_convertible<U_, T_>::value &&
        !( std::is_lvalue_reference<T>::value && !std::is_lvalue_reference<U>::value )
      >::type
    >
    optional( optional<U> && src ) {
      if( src ) {
        impl_.construct( *std::move(src) );
      }
    }
    
    // emplace construct
    template< class... Args,
      class = typename std::enable_if<
        std::is_constructible<T, Args...>::value
      >::type
    >
    optional( emplace_construct_t, Args&&... args ) {
      impl_.construct( std::forward<Args>(args)... );
    }
    template< class... Args,
      class = typename std::enable_if<
        std::is_constructible<T, Args...>::value
      >::type
    >
    optional( bool cond, emplace_construct_t, Args&&... args ) {
      if( cond ) {
        impl_.construct( std::forward<Args>(args)... );
      }
    }
    
    // in_place
    template< class InPlace, class... Dummy,
      class = typename std::enable_if<
        ( sizeof...(Dummy) == 0 ) &&
        etude::is_in_place_applyable<InPlace, T>::value
      >::type
    >
    optional( InPlace && in_place, Dummy... ) {
      impl_.in_place_construct( std::forward<InPlace>(in_place) );
    }
    template< class InPlace, class... Dummy,
      class = typename std::enable_if<
        ( sizeof...(Dummy) == 0 ) &&
        etude::is_in_place_applyable<InPlace, T>::value
      >::type
    >
    optional( bool cond, InPlace && in_place, Dummy... ) {
      impl_.in_place_construct( std::forward<InPlace>(in_place) );
    }
    
    
    // modifiers
    
    // emplace
    // 与えられた引数から直接構築し直す。自己代入チェックは行われない。
    template< class... Args,
      class = typename std::enable_if<
        std::is_constructible<T, Args...>::value
      >::type
    >
    void emplace( Args&&... args ) {
      impl_.construct( std::forward<Args>(args)... );
    }
    
    // assign
    // 与えられた引数を代入する。
    // T に operator= があればそちらを、無いなら一旦破棄してから ctor を呼ぶ。
    // 破棄してから ctor を呼ぶ場合は、自己代入チェック有り
    template< class U = T_&&,
      class = typename std::enable_if<is_assignable_or_convertible<T_, U>::value>::type
    >
    void assign( T_ && x ) {
      impl_.assign( std::forward<T_>(x) );
    }
    template< class U,
      class = typename std::enable_if<
        !std::is_reference<T>::value && // 参照の場合は無効化
        is_assignable_or_convertible<T_, U>::value &&
        !etude::is_in_place_applyable<U, T>::value  // InPlace の場合はそっち優先
      >::type
    >
    void assign( U && x ) {
      impl_.assign( std::forward<U>(x) );
    }
    // in place assignment
    template< class InPlace, class... Dummy,
      class = typename std::enable_if<
        ( sizeof...(Dummy) == 0 ) &&
        etude::is_in_place_applyable<InPlace, T>::value
      >::type
    >
    void assign( InPlace && in_place, Dummy... ) {
      impl_.in_place_construct( std::forward<InPlace>(in_place) );
    }
    
    // assign_if
    template< class P,
      class = typename std::enable_if<
        is_assignable_or_convertible<T_, typename etude::pointee<P>::type>::value
      >::type
    >
    void assign_if( P && p )
    {
      if( p ) {
        impl_.assign( *std::forward<P>(p) );
      }
      else {
        impl_.dispose();
      }
    }
    
    // assignment operators
    // reset
    self_type& operator=( boost::none_t ) {
      impl_.dispose();
      return *this;
    }
    // copy/move は基底クラスのを使う
    // 型変換
    template<class U,
      class U_ = typename std::remove_cv<U>::type,
      class = typename std::enable_if<
        is_assignable_or_convertible<T_, U_ const&>::value
      >::type
    >
    self_type& operator=( optional<U> const& rhs ) {
      assign_if( rhs );
      return *this;
    }
    template<class U,
      class U_ = typename std::remove_cv<U>::type,
      class = typename std::enable_if<
        is_assignable_or_convertible<T_, U_>::value &&
        !( std::is_lvalue_reference<T>::value && !std::is_lvalue_reference<U>::value )
      >::type
    >
    self_type& operator=( optional<U> && rhs ) {
      assign_if( std::move(rhs) );
      return *this;
    }
    // T からの代入
    template< class U = T_&&,
      class = typename std::enable_if<is_assignable_or_convertible<T_, U>::value>::type
    >
    self_type& operator=( T_ && rhs ) {
      assign( std::forward<T>(rhs) );
      return *this;
    }
    template<class U,
      class = typename std::enable_if<
        !std::is_reference<T>::value && ( // 参照の場合は無効化
          is_assignable_or_convertible<T_, U>::value ||
          etude::is_in_place_applyable<U, T>::value
        )
      >::type
    >
    self_type& operator=( U && rhs ) {
      assign( std::forward<U>(rhs) );
      return *this;
    }
    
    // swap
    void swap( self_type& other ) {
      impl_.swap( other.impl_ );
    }
    // 自由関数版
    friend void swap( self_type& one, self_type& another ) {
      one.swap( another );
    }
    
    
    // observers
    
    // 格納された参照を返す（無効参照の場合は未定義動作）
    T& get() {
      BOOST_ASSERT( impl_.get() != 0 );
      return *impl_.get();
    }
    T const& get() const {
      BOOST_ASSERT( impl_.get() != 0 );
      return *impl_.get();
    }
    // 中身に対する rvalue reference を返す（無効参照の場合は未定義、 const は付かない）
    T_&& move() {
      BOOST_ASSERT( impl_.get() != 0 );
      return std::forward<T_>( *impl_.get() );
    }
    // friend 版
    friend T &      get( self_type      & x ) { return x.get(); }
    friend T const& get( self_type const& x ) { return x.get(); }
    friend T_&&     get( self_type     && x ) { return x.move(); }
    
    // 格納されたポインタを返す
    pointer       get_ptr()       { return impl_.get(); }
    const_pointer get_ptr() const { return impl_.get(); }
    // 自由関数版
    friend pointer get_pointer( self_type& x ) {
      return x.get_ptr();
    }
    friend const_pointer get_pointer( self_type const& x ) {
      return x.get_ptr();
    }
    // get のポインタ版
    friend pointer get( self_type* p ) {
      return p ? p->get_ptr() : 0;
    }
    friend const_pointer get( self_type const* p ) {
      return p ? p->get_ptr() : 0;
    }
    
    // operator bool
    explicit operator bool() const {
      return impl_.is_initialized();
    }
    
    // operator->
    pointer operator->() {
      BOOST_ASSERT( impl_.get() != 0 );
      return get_ptr();
    }
    const_pointer operator->() const {
      BOOST_ASSERT( impl_.get() != 0 );
      return get_ptr();
    }
    // operator*
    friend T& operator*( self_type& x ) {
      return x.get();
    }
    friend T const& operator*( self_type const& x ) {
      return x.get();
    }
    // rvalue reference の場合には move する
    friend T_&& operator*( self_type && x ) {
      return x.move();
    }
    
    // boost::optional の get_value_or 互換
    T const& get_value_or( T const& default_ ) const {
      return *this ? **this : default_;
    }
    // move 版
    T_&& move_value_or( T_ && default_ ) {
      if( *this ) {
        return this->move();
      }
      return std::forward<T_>( default_ );
    }
    // friend 版
    friend T const& get_optional_value_or( self_type const& x, T const& default_ ) {
      return x.get_value_or( default_ );
    }
    friend T_ && get_optional_value_or( self_type && x, T_ && default_ ) {
      return x.move_value_or( std::forward<T_>(default_) );
    }
    
    
   private:
    // 演算子多重定義（ friend なので private でよい）
    
    // 元々の T が比較可能か否か
    static bool const is_eq_comp_ = etude::is_equality_comparable<value_type>::value;
    static bool const is_lt_comp_ = etude::is_less_than_comparable<value_type>::value;
    
    // none_t との比較
    friend bool operator==( self_type const& lhs, boost::none_t ) /*noexcept*/ {
      return !lhs;
    }
    friend bool operator< ( self_type const&,     boost::none_t ) /*noexcept*/ {
      return false;
    }
    friend bool operator> ( self_type const& lhs, boost::none_t ) /*noexcept*/ {
      return lhs != boost::none;
    }
    // <=, >= は etude::totally_ordered により自動定義される。
    
    // value_type const& との比較
    // T const& でないのは、 T が参照の場合にもきちんと比較できるようにするため
    template< bool EqualityComparable = is_eq_comp_,
      class = typename std::enable_if<EqualityComparable>::type
    >
    friend bool operator==( self_type const& lhs, value_type const& rhs ) {
      return lhs ? etude::equal_to<value_type>()( lhs.get(), rhs ) : false;
    }
    template< bool LessThanComparable = is_lt_comp_,
      class = typename std::enable_if<LessThanComparable>::type
    >
    friend bool operator< ( self_type const& lhs, value_type const& rhs ) {
      return lhs ? etude::less<value_type>()( lhs.get(), rhs ) : true;
    }
    template< bool LessThanComparable = is_lt_comp_,
      class = typename std::enable_if<LessThanComparable>::type
    >
    friend bool operator> ( self_type const& lhs, value_type const& rhs ) {
      return lhs ? etude::less<value_type>()( rhs, lhs.get() ) : false;
    }
    template< bool LessThanComparable = is_lt_comp_,
      class = typename std::enable_if<LessThanComparable>::type
    >
    friend bool operator<=( self_type const& lhs, value_type const& rhs ) {
      return lhs ? etude::less_equal<value_type>()( lhs.get(), rhs ) : true;
    }
    template< bool LessThanComparable = is_lt_comp_,
      class = typename std::enable_if<LessThanComparable>::type
    >
    friend bool operator>=( self_type const& lhs, value_type const& rhs ) {
      return lhs ? etude::less_equal<value_type>()( rhs, lhs.get() ) : false;
    }
    // 向きを反転したものは etude::partially_ordered により自動定義される。
    
    
  };
  
  // 二つの型の boost::optional 間での比較
  template< class T, class U,
    class = typename std::enable_if<
      etude::is_equality_comparable<T, U>::value
    >::type
  >
  inline bool operator==( optional<T> const& lhs, optional<U> const& rhs )
  {
    return etude::pointee_equal( lhs, rhs );
  }
  template< class T, class U,
    class = typename std::enable_if<
      etude::is_equality_comparable<T, U>::value
    >::type
  >
  inline bool operator!=( optional<T> const& lhs, optional<U> const& rhs ) {
    return !etude::pointee_equal( lhs, rhs );
  }
  template< class T, class U,
    class = typename std::enable_if<
      etude::is_less_than_comparable<T, U>::value
    >::type
  >
  inline bool operator< ( optional<T> const& lhs, optional<U> const& rhs ) {
    return etude::pointee_before( lhs, rhs );
  }
  template< class T, class U,
    class = typename std::enable_if<
      etude::is_less_than_comparable<U, T>::value
    >::type
  >
  inline bool operator> ( optional<T> const& lhs, optional<U> const& rhs ) {
    return etude::pointee_before( rhs, lhs );
  }
  template< class T, class U,
    class = typename std::enable_if<
      etude::is_less_or_equal_comparable<T, U>::value
    >::type
  >
  inline bool operator<=( optional<T> const& lhs, optional<U> const& rhs ) {
    return etude::pointee_less_or_equal( lhs, rhs );
  }
  template< class T, class U,
    class = typename std::enable_if<
      etude::is_less_or_equal_comparable<U, T>::value
    >::type
  >
  inline bool operator>=( optional<T> const& lhs, optional<U> const& rhs ) {
    return etude::pointee_less_or_equal( rhs, lhs );
  }
  
  
  // 構築ヘルパ関数
  
  // 引数から型推論させる
  template<class T,
    class Result = optional<typename decay_and_strip<T>::type>
  >
  inline Result make_optional( T && x ) {
    return { std::forward<T>(x) };
  }
  template<class T,
    class Result = optional<typename decay_and_strip<T>::type>
  >
  inline Result make_optional( bool cond, T && x ) {
    return { cond, std::forward<T>(x) };
  }
  
  // 他の Maybe から構築する
  template< class Maybe,
    class T = typename decay_and_strip<typename pointee<Maybe>::type>::type
  >
  inline optional<T> make_optional_if( Maybe && x )
  {
    if( x ) {
      return { *std::forward<Maybe>(x) };
    }
    return {};
  }
  // 明示的に型を指定
  template< class T, class Maybe,
    class = typename std::enable_if<
      std::is_convertible<typename pointee<Maybe>::type, T>::value
    >::type
  >
  inline optional<T> make_optional_if( Maybe && x )
  {
    if( x ) {
      return { *std::forward<Maybe>(x) };
    }
    return {};
  }


} // namespace etude

#endif  // #ifndef ETUDE_INCLUDED_OPTIONAL_HPP_
