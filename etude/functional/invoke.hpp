//
//  invoke:
//    標準ライブラリ <functional> における INVOKE をシミュレートする関数
// 
//    etude::invoke( f, t1, t2, ..., tN ) は、
//    f がメンバ関数ポインタ／メンバ変数ポインタの場合には
//      std::mem_fn(f)( t1, t2, ..., tN ) を、
//    それ以外の場合には
//      f( t1, t2, ..., tN ) を、
//    それぞれ返します。
//    これは標準ライブラリの <functional> の規格における
//    INVOKE( f, t1, t2, ..., tN ) の動作と同一です。
//    
//    etude::invoke<R>( f, t1, t2, ..., tN ) は、
//    etude::invoke( f, t1, t2, ..., tN ) の戻り値を R に暗黙変換して返します。
//    これは標準ライブラリの <functional> の規格における
//    INVOKE( f, t1, t2, ..., tN, R ) の動作と同一です。
//    ただし R が void の場合、戻り値は暗黙変換ではなく明示的に void に変換されます
//    （つまり、戻り値は単純に無視されます）。
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_FUNCTINAL_INCLUDED_INVOKE_HPP_
#define ETUDE_FUNCTINAL_INCLUDED_INVOKE_HPP_

#include <utility>
#include <type_traits>
#include <functional>

namespace etude {
  
  // 補助関数。メンバ関数／変数ポインタならば std::mem_fn でラップする
  template<class F,
    class = typename std::enable_if<
      std::is_member_pointer<F>::value
    >::type
  >
  inline auto wrap_if_mem_fn( F f )
    -> decltype( std::mem_fn(f) )
  {
    return std::mem_fn(f);
  }
  
  template<class F,
    class = typename std::enable_if<
      !std::is_member_pointer<typename std::decay<F>::type>::value
    >::type
  >
  inline F&& wrap_if_mem_fn( F && f )
  {
    return std::forward<F>(f);
  }
  
  // 本体。 規格 20.8.2 Requirements [func.require] で定義される INVOKE の処理を行う
  template<class F, class... Args,
    class R = decltype(
      etude::wrap_if_mem_fn( std::declval<F>() )( std::declval<Args>()... )
    )
  >
  inline R invoke( F && f, Args&&... args )
  {
    return etude::wrap_if_mem_fn( std::forward<F>(f) )( std::forward<Args>(args)... );
  }
  
  // 型指定版。戻り値を R に暗黙変換する。 R が void ならば単純に捨てる
  template<class R, class F, class... Args,
    class R_ = decltype(
      etude::wrap_if_mem_fn( std::declval<F>() )( std::declval<Args>()... )
    ),
    class = typename std::enable_if<
      std::is_void<R>::value || std::is_convertible<R_, R>::value
    >::type
  >
  inline R invoke( F && f, Args&&... args )
  {
    return static_cast<R>(
      etude::wrap_if_mem_fn( std::forward<F>(f) )( std::forward<Args>(args)... )
    );
  }
  
} // namespace etude

#endif  // #ifndef ETUDE_FUNCTINAL_INCLUDED_INVOKE_HPP_
