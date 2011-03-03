//
//  optional:
//    Boost.Optional の C++0x 拡張
// 
//    for Boost.勉強会 #4
//    
//    TODO: lazy 版の get_optional_value_or を実装する
//          テストを書く
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_INCLUDED_OPTIONAL_HPP_
#define ETUDE_INCLUDED_OPTIONAL_HPP_

#include <utility>
#include <type_traits>

#include <boost/operators.hpp>
#include <boost/none.hpp>
#include <boost/assert.hpp>
#include <boost/utility/addressof.hpp>

#include "types/decay_and_strip.hpp"
#include "types/is_assignable.hpp"
#include "types/bool_constant.hpp"
#include "memory/storage.hpp"
#include "memory/apply_in_place.hpp"
#include "utility/compressed_pair.hpp"
#include "utility/emplace_construct.hpp"

namespace etude {

  template<class T>
  class optional;

  template<class T>
  struct optional_impl_base_
  {
    // construct
    optional_impl_base_()
      : impl_() {}  // impl_ の ctor で first は初期化されて false になる
    // copy/move construct
    optional_impl_base_( optional_impl_base_ const& src )
      : impl_()
    {
      if( auto const p = src.get() ) {
        construct( *p );
      }
    }
    optional_impl_base_( optional_impl_base_ && src )
      : impl_()
    {
      if( auto const p = src.get() ) {
        construct( std::forward<T>(*p) );
      }
    }
    // copy/move assign
    optional_impl_base_& operator=( optional_impl_base_ const& src )
    {
      if( auto const p = src.get() ) {
        this->assign( *p );
      }
      else {
        this->dispose();
      }
      return *this;
    }
    optional_impl_base_& operator=( optional_impl_base_ && src )
    {
      if( auto const p = src.get() ) {
        this->assign( std::forward<T>(*p) );
      }
      else {
        this->dispose();
      }
      return *this;
    }
    
    // observers
    bool is_initialized() const { return impl_.first(); }
    
    void*       address()       { return impl_.second().address(); }
    void const* address() const { return impl_.second().address(); }
    
    T* get() {
      return is_initialized() ? static_cast<T*>( address() ) : 0;
    }
    T const* get() const {
      return is_initialized() ? static_cast<T const*>( address() ) : 0;
    }
    
    // modifiers
    void dispose()
    {
      if( T* p = this->get() ) {
        p->~T();
        impl_.first() = false;
      }
    }
    
    template<class... Args>
    void construct( Args&&... args )
    {
      dispose();
      
      ::new( address() ) T( std::forward<Args>(args)... );
      impl_.first() = true;
    }
    
    template<class InPlace>
    void in_place_construct( InPlace && in_place )
    {
      dispose();
      
      apply_in_place<T>( std::forward<InPlace>(in_place), address() );
      impl_.first() = true;
    }
    
    template<class U>
    void assign( U && x )
    {
      return assign_( std::forward<U>(x), 0 );
    }
    
    // swap
    void swap( optional_impl_base_& other )
    {
      if( auto const p1 = this->get() ) {
        if( auto const p2 = other.get() ) {
          using std::swap;
          swap( *p1, *p2 );
        }
        else {
          other.construct( std::forward<T>(*p2) );
          this->dispose();
        }
      }
      else {
        if( auto const p2 = other.get() ) {
          this->construct( std::forward<T>(*p2) );
          other.dispose();
        }
      }
    }
    
   private:
    typedef etude::storage<T> storage_type;
    etude::compressed_pair<bool, storage_type> impl_;
    
    // assign の実装
    // assignable ならば代入を使う
    template< class U,
      class = typename std::enable_if<
        etude::is_assignable<T, U>::value
      >::type
    >
    void assign_( U && x, int )
    {
      if( auto const p = this->get() ) {
        *p = std::forward<U>(x);
      }
      else {
        construct( std::forward<U>(x) );
      }
    }
    // assignable でないなら construct を使う
    template<class U>
    void assign_( U && x, ... )
    {
      construct( std::forward<U>(x) );
    }
    
  };

  template<class T, class = void>
  struct optional_impl_
    : optional_impl_base_<T>
  {
    typedef optional_impl_base_<T> base_;
    
    optional_impl_() = default;
    ~optional_impl_(){ base_::dispose(); }
    
  };
  
  template<class T>
  struct optional_impl_< T,
    typename std::enable_if<
      !std::is_reference<T>::value &&
      std::has_trivial_destructor<T>::value
    >::type
  >
    : optional_impl_base_<T>
  {
    typedef optional_impl_base_<T> base_;
    
    optional_impl_() = default;
    // ~optional_impl_(){ base_::dispose(); }
    
  };
  
  template<class T>
  struct optional_impl_< T,
    typename std::enable_if<std::is_reference<T>::value>::type
  >
  {
    typedef typename std::add_pointer<T>::type pointer;
    
    optional_impl_()
      : p_() {}
    
    bool is_initialized() const { return p_; }
    pointer get() const { return p_; }
    
    template<class U>
    void construct( U && x ) {
      p_ = boost::addressof(x);
    }
    
    template<class InPlace>
    void in_place_construct( InPlace && in_place ) = delete;
    
    template< class U,
      class = typename std::enable_if<
        std::is_convertible<U, T>::value
      >::type
    >
    void assign( U && x ) {
      p_ = boost::addressof(x);
    }
    
    void dispose() {
      p_ = 0;
    }
    
    void swap( optional_impl_& other ) {
      std::swap( p_, other.p_ );
    }
    
   private:
    pointer p_;
  };
  
  // メタ関数 optional_assignable
  template<class T, class U>
  struct optional_assignable_ :
    etude::bool_constant<
      std::is_convertible<U, T>::value || etude::is_assignable<T, U>::value
    >
  {};

  // 実装本体
  template<class T>
  class optional
    : boost::totally_ordered<optional<T>,
        boost::totally_ordered<optional<T>, T,
          boost::totally_ordered<optional<T>, boost::none_t>
        >
      >
  {
    typedef optional<T> self_type;
    typedef typename std::remove_const<T>::type T_;
    typedef optional_impl_<T_> impl_type;
    struct dummy_ {};
    
   public:
    typedef T_   value_type;  // value_type   は const が付かない
    typedef T  element_type;  // element_type は const が付くかも
    
    typedef T&             reference;
    typedef T const& const_reference;
    typedef T_&&    rvalue_reference;
    
    typedef typename std::add_pointer<T>::type             pointer;
    typedef typename std::add_pointer<T const>::type const_pointer;
    
    // 無効値
    optional() {}
    optional( boost::none_t ) {}
    
    // T_ からの構築
    optional( T_ && x ) {
      impl_.construct( std::forward<T_>(x) );
    }
    // 型変換構築
    template<class U,
      class = typename std::enable_if<std::is_convertible<U, T>::value>::type
    >
    optional( U && x ) {
      impl_.construct( std::forward<U>(x) );
    }
    
    // 条件付き構築
    optional( bool cond, T_ && x ) {
      if( cond ) {
        impl_.construct( std::forward<T_>(x) );
      }
    }
    template<class U,
      class = typename std::enable_if<std::is_convertible<U, T>::value>::type
    >
    optional( bool cond, U && x ) {
      if( cond ) {
        impl_.construct( std::forward<U>(x) );
      }
    }
    
    // copy/move は基底クラスのを使う
    /*
    optional( optional const& ) = default;
    optional( optional && )     = default;
    */
    
    // 型変換
    template< class U,
      class = typename std::enable_if<
        std::is_convertible<U const&, T>::value
      >::type
    >
    optional( optional<U> const& src ) {
      if( src ) {
        impl_.construct( *src );
      }
    }
    template< class U,
      class = typename std::enable_if<
        std::is_convertible<U, T>::value
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
    template< class InPlace,
      class = typename std::enable_if<
        etude::is_in_place_applyable<InPlace, T>::value
      >::type
    >
    optional( InPlace && in_place, dummy_ = dummy_() ) {
      impl_.in_place_construct( std::forward<InPlace>(in_place) );
    }
    template< class InPlace,
      class = typename std::enable_if<
        etude::is_in_place_applyable<InPlace, T>::value
      >::type
    >
    optional( bool cond, InPlace && in_place, dummy_ = dummy_() ) {
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
    void assign( T_ && x ) {
      impl_.assign( std::forward<T_>(x) );
    }
    template< class U,
      class = typename std::enable_if<
        optional_assignable_<T_, U>::value
      >::type
    >
    void assign( U && x ) {
      impl_.assign( std::forward<U>(x) );
    }
    // in place assignment
    template< class InPlace,
      class = typename std::enable_if<
        !optional_assignable_<T_, InPlace>::value &&
        etude::is_in_place_applyable<InPlace, T>::value
      >::type
    >
    void assign( InPlace && in_place, dummy_ = dummy_() ) {
      impl_.in_place_construct( std::forward<InPlace>(in_place) );
    }
    
    // assign_if
    template< class P,
      class = decltype( bool( std::declval<P>() ) ),
      class U = decltype( *std::declval<P>() ),
      class = typename std::enable_if<
        optional_assignable_<T_, U>::value
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
      class = typename std::enable_if<
        optional_assignable_<T_, U>::value
      >::type
    >
    self_type& operator=( optional<U> const& rhs ) {
      assign_if( rhs );
      return *this;
    }
    template<class U,
      class = typename std::enable_if<
        optional_assignable_<T_, U>::value
      >::type
    >
    self_type& operator=( optional<U> && rhs ) {
      assign_if( std::move(rhs) );
      return *this;
    }
    // T からの代入
    self_type& operator=( T_ && rhs ) {
      assign( std::forward<T>(rhs) );
      return *this;
    }
    template<class U,
      class = typename std::enable_if<
        optional_assignable_<T_, U>::value ||
        etude::is_in_place_applyable<U, T>::value
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
    
    // get はポインタを返す（ boost::optional とは違う）
    pointer       get()       { return impl_.get(); }
    const_pointer get() const { return impl_.get(); }
    // 互換性のため get_ptr も用意
    pointer          get_ptr()       { return get(); }
    const_pointer    get_ptr() const { return get(); }
    // 自由関数版
    friend pointer get_pointer( self_type& x ) {
      return x.get_ptr();
    }
    friend const_pointer get_pointer( self_type const& x ) {
      return x.get_ptr();
    }
    
    // operator bool
    explicit operator bool() const {
      return impl_.is_initialized();
    }
    
    // operator->
    pointer operator->() {
      BOOST_ASSERT( !!*this );
      return get();
    }
    const_pointer operator->() const {
      BOOST_ASSERT( !!*this );
      return get();
    }
    // operator*
    friend T& operator*( self_type& x ) {
      BOOST_ASSERT( !!x );
      return *x.get();
    }
    friend T const& operator*( self_type const& x ) {
      BOOST_ASSERT( !!x );
      return *x.get();
    }
    // rvalue reference の場合には、 const を外した上で move する
    friend T_&& operator*( self_type && x ) {
      BOOST_ASSERT( !!x );
      return std::forward<T_>( *x.get_() );
    }
    
    // boost::optional の get_value_or 互換
    T const& get_value_or( T const& default_ ) const {
      return get() ? *get() : default_;
    }
    
    
    // 比較
    
    // none_t との比較
    friend bool operator==( self_type const& lhs, boost::none_t ) /*noexcept*/ {
      return !lhs;
    }
    friend bool operator< ( self_type const&,     boost::none_t ) /*noexcept*/ {
      return false;
    }
    friend bool operator> ( self_type const& lhs, boost::none_t ) /*noexcept*/ {
      return !!lhs;
    }
    
    // T const& との比較
    friend bool operator==( self_type const& lhs, T const& rhs ) /*noexcept*/ {
      return lhs ? (*lhs == rhs) : false;
    }
    friend bool operator< ( self_type const& lhs, T const& rhs ) /*noexcept*/ {
      return !lhs ? true  : ( *lhs < rhs );
    }
    friend bool operator> ( self_type const& lhs, T const& rhs ) /*noexcept*/ {
      return !lhs ? false : ( rhs < *lhs );
    }
    
    // optional 同士の相互比較
    friend bool operator==( self_type const& lhs, self_type const& rhs ) /*noexcept*/ {
      return rhs ? ( lhs == *rhs ) : ( lhs == boost::none );
    }
    friend bool operator< ( self_type const& lhs, self_type const& rhs ) /*noexcept*/ {
      return rhs ? ( lhs <  *rhs ) : ( lhs <  boost::none );
    }
    
    
    
    // !=, >, <=, >= は boost::totally_ordered により自動定義される
    
   private:
    impl_type impl_;
    
    typename std::add_pointer<T_>::type get_() { return impl_.get(); }
    
  };
  
  
  // 取得
  template<class T>
  inline T& get_optional_value_or( optional<T> const& x, T const& default_ ) {
    return x.get_value_or( default_ );
  }
  
  
  // ヘルパ関数
  template<class T,
    class Result = optional<typename decay_and_strip<T>::type>
  >
  inline Result make_optional( T && x ) {
    return Result( std::forward<T>(x) );
  }
  template<class T,
    class Result = optional<typename decay_and_strip<T>::type>
  >
  inline Result make_optional( bool cond, T && x ) {
    return Result( cond, std::forward<T>(x) );
  }

} // namespace etude

#endif  // #ifndef ETUDE_INCLUDED_OPTIONAL_HPP_
