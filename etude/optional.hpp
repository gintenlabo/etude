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

#include "types/is_equality_comparable.hpp"
#include "types/is_less_than_comparable.hpp"
#include "types/is_less_or_equal_comparable.hpp"
#include "types/is_assignable.hpp"
#include "operators/totally_ordered.hpp"
#include "operators/partially_ordered.hpp"

#include "utility/empty_base.hpp"
#include "noncopyable.hpp"
#include "immovable.hpp"

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
    // 自己代入になりうる場合はチェックする
    template< class U,
      class = typename std::enable_if<
        !etude::is_assignable<T, U>::value &&
        std::is_convertible<typename std::decay<U>::type*, T*>::type
      >::type
    >
    void assign_( U && x, ... )
    {
      if( boost::addressof(x) != this->get() ) {
        construct( std::forward<U>(x) );
      }
    }
    // それ以外
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

  // メタ関数 optional_base_
  // T の copy/move の有無に合わせて optional の copy/move の有無を変える
  template<class T>
  struct optional_base_
  {
    typedef etude::empty_base< etude::optional<T> > base;
    
    typedef typename std::conditional<
      !std::is_constructible<T, T&&>::value, etude::immovable<base>,
      typename std::conditional<
        !std::is_constructible<T, T const&>::value,
        etude::noncopyable<base>, base
      >::type
    >::type type;
  
  };

  // 実装本体
  template<class T>
  class optional
    : etude::totally_ordered< optional<T>, boost::none_t,
        etude::partially_ordered< optional<T>, T,
          etude::partially_ordered1< optional<T>,
            typename optional_base_<T>::type
          >
        >
      >
  {
    typedef optional<T> self_type;
    typedef typename std::remove_const<T>::type T_;
    typedef optional_impl_<T_> impl_type;
    struct dummy_ { explicit dummy_(){} };
    
    static bool const eq_comparable = etude::is_equality_comparable<T>::value;
    static bool const lt_comparable = etude::is_less_than_comparable<T>::value;
    static bool const le_comparable = etude::is_less_or_equal_comparable<T>::value;
    
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
        std::is_convertible<U, T>::value
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
        std::is_convertible<U, T>::value
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
    template< class U = T_&&,
      class = typename std::enable_if<optional_assignable_<T_, U>::value>::type
    >
    void assign( T_ && x ) {
      impl_.assign( std::forward<T_>(x) );
    }
    template< class U,
      class = typename std::enable_if<optional_assignable_<T_, U>::value>::type
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
    template< class U = T_&&,
      class = typename std::enable_if<optional_assignable_<T_, U>::value>::type
    >
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
    
    
    // 比較
    
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
    
    // T const& との比較
    template< bool EqualityComparable = eq_comparable,
      class = typename std::enable_if<EqualityComparable>::type
    >
    friend bool operator==( self_type const& lhs, T const& rhs ) /*noexcept*/ {
      return lhs ? bool( *lhs == rhs ) : false;
    }
    template< bool LessThanComparable = lt_comparable,
      class = typename std::enable_if<LessThanComparable>::type
    >
    friend bool operator< ( self_type const& lhs, T const& rhs ) /*noexcept*/ {
      return lhs ? bool( *lhs <  rhs ) : true;
    }
    template< bool LessThanComparable = lt_comparable,
      class = typename std::enable_if<LessThanComparable>::type
    >
    friend bool operator> ( self_type const& lhs, T const& rhs ) /*noexcept*/ {
      return lhs ? bool(  rhs < *lhs ) : false;
    }
    template< bool LessOrEqualComparable = le_comparable,
      class = typename std::enable_if<LessOrEqualComparable>::type
    >
    friend bool operator<=( self_type const& lhs, T const& rhs ) /*noexcept*/ {
      return lhs ? bool( *lhs <=  rhs ) : true;
    }
    template< bool LessOrEqualComparable = le_comparable,
      class = typename std::enable_if<LessOrEqualComparable>::type
    >
    friend bool operator>=( self_type const& lhs, T const& rhs ) /*noexcept*/ {
      return lhs ? bool(  rhs <= *lhs ) : false;
    }
    // 向きを反転したものは etude::partially_ordered により自動定義される。
    
    // optional 同士の相互比較
    template< bool EqualityComparable = eq_comparable,
      class = typename std::enable_if<EqualityComparable>::type
    >
    friend bool operator==( self_type const& lhs, self_type const& rhs ) /*noexcept*/ {
      return rhs ? ( lhs == *rhs ) : ( lhs == boost::none );
    }
    template< bool LessThanComparable = lt_comparable,
      class = typename std::enable_if<LessThanComparable>::type
    >
    friend bool operator< ( self_type const& lhs, self_type const& rhs ) /*noexcept*/ {
      return rhs ? ( lhs <  *rhs ) : ( lhs <  boost::none );
    }
    template< bool LessOrEqualComparable = le_comparable,
      class = typename std::enable_if<LessOrEqualComparable>::type
    >
    friend bool operator<=( self_type const& lhs, self_type const& rhs ) /*noexcept*/ {
      return rhs ? ( lhs <= *rhs ) : ( lhs <= boost::none );
    }
    // 向きを反転したものは etude::partially_ordered により自動定義される。
    
    
   private:
    impl_type impl_;
    
  };
  
  
  // 取得
  template<class T>
  inline T const& get_optional_value_or( optional<T> const& x, T const& default_ ) {
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
