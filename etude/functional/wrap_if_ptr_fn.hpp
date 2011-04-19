//
//  wrap_if_ptr_fn:
//    関数ポインタ／メンバポインタならば etude::ptr_fn でラップする
// 
//    etude::wrap_if_ptr_fn( f ) は、 f が関数ポインタあるいはメンバへのポインタならば
//    std::ptr_fn( f ) を、そうでなければ f を返します。
//    
//    これを使うことで、 f をクラスに変換させることが可能になり、
//    また適切な result_type や argument_type などが定義されます。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_FUNCTINAL_INCLUDED_WRAP_IF_PTR_FN_HPP_
#define ETUDE_FUNCTINAL_INCLUDED_WRAP_IF_PTR_FN_HPP_

#include <utility>
#include <type_traits>
#include <functional>

#include "ptr_fn.hpp"
#include "../types/is_function_pointer.hpp"

namespace etude {
  
  // メンバ関数／変数ポインタならば std::mem_fn でラップする
  template< class F,
    class = typename std::enable_if<
      std::is_member_pointer<F>::value || etude::is_function_pointer<F>::value
    >::type
  >
  inline auto wrap_if_ptr_fn( F f )
    -> decltype( etude::ptr_fn(f) )
  {
    return etude::ptr_fn(f);
  }
  
  // そうでなければ引数をそのまま返す
  template<class F,
    class F_ = typename std::decay<F>::type,
    class = typename std::enable_if<
      !std::is_member_pointer<F_>::value && !etude::is_function_pointer<F_>::value
    >::type
  >
  inline F&& wrap_if_ptr_fn( F && f )
  {
    return std::forward<F>(f);
  }
  
} // namespace etude

#endif  // #ifndef ETUDE_FUNCTINAL_INCLUDED_WRAP_IF_PTR_FN_HPP_
