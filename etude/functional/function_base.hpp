//
//  function_base:
//    result_type, argument_type, first_argument_type, second_argument_type を自動定義
// 
//    与えられたシグネチャ（関数形式）に対応する各種 typedef を提供します。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_FUNCTINAL_INCLUDED_FUNCTION_BASE_HPP_
#define ETUDE_FUNCTINAL_INCLUDED_FUNCTION_BASE_HPP_

namespace etude {
  
  template<class Singature, class Tag = void>
  class function_base;
  
  // 引数一つの関数なら result_type と argument_type を定義
  template<class R, class Arg, class Tag>
  struct function_base<R (Arg), Tag> {
    typedef Arg argument_type;
    typedef R result_type;
  };
  
  // 引数が二つなら result_type, first_argument_type, second_argument_type を定義
  template<class R, class Arg1, class Arg2, class Tag>
  struct function_base<R (Arg1, Arg2), Tag> {
    typedef Arg1 first_argument_type;
    typedef Arg2 second_argument_type;
    typedef R result_type;
  };
  
  // それ以外の関数の場合は result_type のみ定義
  template<class R, class... Args, class Tag>
  struct function_base<R (Args...), Tag> {
    typedef R result_type;
  };
  
  // ... に対しても一応 対応する
  template<class R, class... Args, class Tag>
  struct function_base< R ( Args..., ... ), Tag > {
    typedef R result_type;
  };
  
} // namespace etude

#endif  // #ifndef ETUDE_FUNCTINAL_INCLUDED_FUNCTION_BASE_HPP_
