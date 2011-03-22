//
//  wrap_if_mem_fn:
//    メンバポインタならば std::mem_fn を呼ぶ
// 
//    etude::wrap_if_mem_fn( f ) は、 f がメンバへのポインタならば
//    std::mem_fn( f ) を、そうでなければ f を返します。
//    
//    これを使うことで、標準ライブラリ <functional> の規格において定義された
//    INVOKE( f, t1, ..., tN ) は、 etude::wrap_if_mem_fn(f)( t1, ..., tN )
//    と機械的に書き換えることが可能になります。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_FUNCTINAL_INCLUDED_WRAP_IF_MEM_FN_HPP_
#define ETUDE_FUNCTINAL_INCLUDED_WRAP_IF_MEM_FN_HPP_

#include <utility>
#include <type_traits>
#include <functional>

namespace etude {
  
  // メンバ関数／変数ポインタならば std::mem_fn でラップする
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
  
  // そうでなければ引数をそのまま返す
  template<class F,
    class = typename std::enable_if<
      !std::is_member_pointer<typename std::decay<F>::type>::value
    >::type
  >
  inline F&& wrap_if_mem_fn( F && f )
  {
    return std::forward<F>(f);
  }
  
} // namespace etude

#endif  // #ifndef ETUDE_FUNCTINAL_INCLUDED_WRAP_IF_MEM_FN_HPP_
