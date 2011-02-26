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
#include "memory/storage.hpp"
#include "memory/apply_in_place.hpp"
#include "utility/compressed_pair.hpp"
#include "utility/emplace_construct.hpp"

namespace etude {

  template<class T>
  struct optional_impl_base_
  {
    optional_impl_base_() {
      impl_.first() = false;
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
      bool& initialized = impl_.first();
      
      if( initialized ) {
        T* p = get();
        p->~T();
        initialized = false;
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
    
   private:
    typedef etude::storage<T> storage_type;
    etude::compressed_pair<bool, storage_type> impl_;
    
  };

  template<class T, class = void>
  struct optional_impl_
    : optional_impl_base_<T>
  {
    typedef optional_impl_base_<T> base_;
    
    using base_::is_initialized;
    using base_::get;
    
    using base_::construct;
    using base_::in_place_construct;
    using base_::dispose;
    
    
    optional_impl_(){}
    
    template<class... Args>
    explicit optional_impl_( Args&&... args )
    {
      construct( std::forward<Args>(args)... );
    }
    
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
    
    using base_::is_initialized;
    using base_::get;
    
    using base_::construct;
    using base_::in_place_construct;
    using base_::dispose;
    
    
    optional_impl_(){}
    
    template<class... Args>
    explicit optional_impl_( Args&&... args )
    {
      construct( std::forward<Args>(args)... );
    }
    
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
    
    void dispose() {
      p_ = 0;
    }
    
   private:
    pointer p_;
  };

  template<class T>
  class optional
    : boost::totally_ordered<optional<T>,
        boost::totally_ordered<optional<T>, T,
          boost::totally_ordered<optional<T>, boost::none_t>
        >
      >
  {
    typedef optional<T> self_type;
    struct dummy_ {};
    
   public:
    typedef T    value_type;
    typedef T  element_type;
    typedef T&             reference;
    typedef T const& const_reference;
    typedef typename std::add_pointer<T>::type             pointer;
    typedef typename std::add_pointer<T const>::type const_pointer;
    
    // 無効値
    optional() {}
    optional( boost::none_t ) {}
    
    // T からの構築
    optional( T && x ) {
      impl_.construct( std::forward<T>(x) );
    }
    // 型変換構築
    template<class U,
      class = typename std::enable_if<std::is_convertible<U, T>::value>::type
    >
    optional( U && x ) {
      impl_.construct( std::forward<U>(x) );
    }
    
    // 条件付き構築
    optional( bool cond, T && x ) {
      if( cond ) {
        impl_.construct( std::forward<T>(x) );
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
    
    // copy/move
    optional( self_type const& src ) {
      if( src ) {
        impl_.construct( *src );
      }
    }
    optional( self_type && src ) {
      if( src ) {
        impl_.construct( *std::move(src) );
      }
    }
    
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
    
    // assign_if
    template< class P,
      class = typename std::enable_if<
        std::is_convertible<decltype( *std::declval<P>() ), T>::value
      >::type
    >
    void assign_if( P && p ) {
      if( p ) {
        impl_.construct( *std::forward<P>(p) );
      }
      else {
        impl_.dispose();
      }
    }
    
    // assignment operators
    self_type& operator=( boost::none_t ) {
      impl_.dispose();
      return *this;
    }
    self_type& operator=( self_type const& rhs ) {
      assign_if( rhs );
      return *this;
    }
    self_type& operator=( self_type && rhs ) {
      assign_if( std::move(rhs) );
      return *this;
    }
    template<class U,
      class = typename std::enable_if<
        std::is_convertible<U, T>::value
      >::type
    >
    self_type& operator=( optional<U> const& rhs ) {
      assign_if( rhs );
      return *this;
    }
    template<class U,
      class = typename std::enable_if<
        std::is_convertible<U, T>::value
      >::type
    >
    self_type& operator=( optional<U> && rhs ) {
      assign_if( std::move(rhs) );
      return *this;
    }
    self_type& operator=( T && rhs ) {
      impl_.construct( std::forward<T>(rhs) );
      return *this;
    }
    template<class U,
      class = typename std::enable_if<
        std::is_convertible<U, T>::value
      >::type
    >
    self_type& operator=( U && rhs ) {
      impl_.construct( std::forward<U>(rhs) );
      return *this;
    }
    
    // swap
    void swap( self_type& other ) {
      if( *this ) {
        if( other ) {
          using std::swap;
          swap( **this, *other );
        }
        else {
          other = *std::move(*this);
          *this = boost::none;
        }
      }
      else {
        if( other ) {
          *this = *std::move(other);
          other = boost::none;
        }
      }
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
    pointer       get_ptr()       { return get(); }
    const_pointer get_ptr() const { return get(); }
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
      BOOST_ASSERT( get() != 0 );
      return get();
    }
    const_pointer operator->() const {
      BOOST_ASSERT( get() != 0 );
      return get();
    }
    // operator*
    friend T& operator*( self_type& x ) {
      BOOST_ASSERT( x.get() != 0 );
      return *x.get();
    }
    friend T const& operator*( self_type const& x ) {
      BOOST_ASSERT( x.get() != 0 );
      return *x.get();
    }
    friend T&& operator*( self_type && x ) {
      BOOST_ASSERT( x.get() != 0 );
      return std::forward<T>( *x.get() );
    }
    
    // boost::optional の get_value_or 互換
    T const& get_value_or( T const& default_ ) const {
      return get() ? *get() : default_;
    }
    
    // 比較
    friend bool operator==( self_type const& lhs, self_type const& rhs ) /*noexcept*/ {
      return lhs ?
        ( rhs ? (*lhs == *rhs) : false ) :
        ( rhs ? false : true );
    }
    friend bool operator<( self_type const& lhs, self_type const& rhs ) /*noexcept*/ {
      return !rhs ? false : (
        !lhs ? true : ( *lhs < *rhs )
      );
    }
    
    friend bool operator==( self_type const& lhs, T const& rhs ) /*noexcept*/ {
      return lhs ? (*lhs == rhs) : false;
    }
    friend bool operator<( self_type const& lhs, T const& rhs ) /*noexcept*/ {
      return !lhs ? true : ( *lhs < rhs );
    }
    friend bool operator>( self_type const& lhs, T const& rhs ) /*noexcept*/ {
      return !lhs ? false : ( rhs < *lhs );
    }
    
    friend bool operator==( self_type const& lhs, boost::none_t ) /*noexcept*/ {
      return !lhs;
    }
    friend bool operator<( self_type const&, boost::none_t ) /*noexcept*/ {
      return false;
    }
    friend bool operator>( self_type const& lhs, boost::none_t ) /*noexcept*/ {
      return bool(lhs);
    }
    
    // !=, >, <=, >= は boost::totally_ordered により自動定義される
    
   private:
    optional_impl_<typename std::remove_const<T>::type> impl_;
    
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
