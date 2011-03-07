//
//  value_holder:
//    タプルを unpack しながら構築できるラッパークラス
// 
//    単独では特に意味はありませんが、メンバとして使うときに便利です。
//    
//  Copyright (C) 2010-11  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_UTILITY_INCLUDED_VALUE_HOLDER_HPP_
#define ETUDE_UTILITY_INCLUDED_VALUE_HOLDER_HPP_

#include <type_traits>
#include <utility>
#include <boost/utility/addressof.hpp>

#include "holder.hpp"
#include "emplace_construct.hpp"
#include "unpack_construct.hpp"
#include "unpacked_tuple.hpp"

#include "../types/indices.hpp"
#include "tuple_get.hpp"
#include "../types/tuple_types.hpp"
#include "../types/tuple_indices.hpp"
#include "../types/is_convertible.hpp"

namespace etude {
 namespace value_holder_ { // ADL 回避
 
  // 直接 etude::holder を使うのはアレなので
  // 実装用クラスを持ってくる
  using etude::holder_::holder_;
  
  // タプルの unpack をするにはもう一段実装用クラスをはさむ必要あり
  template<class T, class Tag>
  class value_holder_
    : private holder_<T, Tag>
  {
    typedef holder_<T, Tag> base;
    
   public:
    using base::get;
    using base::move;
    
    // デフォルト構築
    value_holder_() = default;
    
    // pack されてない引数から構築
    template<class... Args>
    value_holder_( emplace_construct_t, Args&&... args )
      : base( std::forward<Args>(args)... ) {}
    
    // pack された引数から構築
    template<class Tuple, std::size_t... Indices>
    value_holder_( Tuple && t, etude::indices<Indices...> )
      : base ( tuple_forward<Indices, Tuple>(t)... )
    {
      (void)t;  // unused variable 警告避け（ Tuple が空の場合に）
    }
    
    // gcc4.5.0 では implicit move が実装されていないので
    value_holder_( value_holder_ const& ) = default;
    value_holder_( value_holder_ && )     = default;
    
    // swap
    void swap( value_holder_& x ) {
      base::swap( static_cast<base&>(x) );
    }
    
  };
  
  // 本体
  template<class T, class Tag = void>
  class value_holder
    : private value_holder_<typename std::remove_const<T>::type, Tag>
  {
    typedef typename std::remove_const<T>::type T_;
    typedef value_holder_<T_, Tag> base;
    typedef value_holder<T, Tag> self_type;
    
   public:
    // デフォルト構築
    value_holder() = default;
    
    // T からの構築
    template<class U = T_&&,
      class = typename std::enable_if<
        std::is_constructible<T, U>::value
      >::type
    >
    value_holder( T_ && src )
      : base( emplace_construct, std::forward<T_>(src) ) {}
    // 型変換構築（ const T& も含む）
    template<class U,
      class = typename std::enable_if<
        std::is_convertible< U, holder<T> >::value
      >::type
    >
    value_holder( U && src )
      : base( emplace_construct, std::forward<U>(src) ) {}
    
    // pack されてない引数から構築
    template<class... Args,
      class = typename std::enable_if<
        std::is_constructible<holder<T>, Args...>::value
      >::type
    >
    value_holder( emplace_construct_t, Args&&... args )
      : base( emplace_construct, std::forward<Args>(args)... ) {}
    
    // pack された引数から構築
    template<class Tuple,
      class = typename std::enable_if<
        etude::is_convertible<
          typename etude::tuple_types<Tuple>::type, holder<T>
        >::value
      >::type
    >
    value_holder( unpack_construct_t, Tuple && t )
      : base( std::forward<Tuple>(t), etude::tuple_indices<Tuple>() ) {}
    
    // unpack された引数から構築
    template<class Tuple, std::size_t... Indices,
      class = typename std::enable_if<
        etude::is_convertible<
          typename etude::tuple_types<Tuple>::type, holder<T>
        >::value
      >::type
    >
    explicit value_holder( unpacked_tuple<Tuple, Indices...> t )
      : base( emplace_construct, etude::move<Indices>(t)... ) {}
    
    // gcc4.5.0 では implicit move が（ｒｙ
    value_holder( self_type const& ) = default;
    value_holder( self_type && )     = default;
    
    // 型変換コンストラクタ
    // copy
    template<class U, class Tag_,
      class U_ = typename std::remove_const<U>::type,
      class = typename std::enable_if<
        std::is_convertible<U_, T>::value
      >::type
    >
    value_holder( value_holder<U, Tag_> const& src )
      : base( emplace_construct, src.get() ) {}
    // move
    template<class U, class Tag_,
      class U_ = typename std::remove_const<U>::type,
      class = typename std::enable_if<
        std::is_convertible<U_, T>::value
      >::type
    >
    value_holder( value_holder<U, Tag_> && src )
      : base( emplace_construct, src.move() ) {}
    
    
    // 再代入
    // 単純再代入
    template< class U = T_,
      class = typename std::enable_if<
        etude::is_assignable<T_, U>::value
      >::type
    >
    value_holder& operator=( T_ && x ) {
      base::get() = std::forward<T_>(x);
      return *this;
    }
    template< class U,
      class = typename std::enable_if<
        etude::is_assignable<T_, U>::value
      >::type
    >
    value_holder& operator=( U && x ) {
      base::get() = std::forward<U>(x);
      return *this;
    }
    
    // copy
    template<class U, class Tag_,
      class = typename std::enable_if<
        etude::is_assignable<T_, U const&>::value
      >::type
    >
    value_holder& operator=( value_holder<U, Tag_> const& rhs ) {
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
    value_holder& operator=( value_holder<U, Tag_> && rhs ) {
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
    friend T &      operator*( value_holder &      x ){ return x.get(); }
    friend T const& operator*( value_holder const& x ){ return x.get(); }
    friend T_ &&    operator*( value_holder &&     x ){ return x.move(); }
    
    // operator->
    typename std::add_pointer<T>::type operator->() {
      return boost::addressof( get() );
    }
    typename std::add_pointer<T const>::type operator->() const {
      return boost::addressof( get() );
    }
    
    // swap
    void swap( self_type& x ) {
      base::swap( static_cast<base&>(x) );
    }
    
  };
  
  // 自由関数版 get
  template<class T, class Tag>
  inline T& get( value_holder<T, Tag>& x ) {
    return x.get();
  }
  template<class T, class Tag>
  inline T const& get( value_holder<T, Tag> const& x ) {
    return x.get();
  }
  // move 版
  template<class T, class Tag>
  inline typename std::remove_const<T>::type && get( value_holder<T, Tag> && x ) {
    return x.move();
  }
  
  // 自由関数版 swap
  template<class T, class Tag>
  inline void swap( value_holder<T, Tag>& one, value_holder<T, Tag>& another ) {
    one.swap( another );
  }
 
 }  // namespace value_holder_
 using namespace value_holder_;
}

#endif  // #ifndef ETUDE_UTILITY_INCLUDED_VALUE_HOLDER_HPP_
