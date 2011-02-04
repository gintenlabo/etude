//
//  is_callable :
//    関数呼び出し可能ならば true_type になるメタ関数
//    
//    is_callable<T, R (Args...)> は、 T 型の t 及び Args... 型の args... に対し
//    INVOKE( t, args... ) が well-formed で R 型に暗黙変換できる場合には
//    std::true_type を、そうでない場合には std::false_type を継承します。
//    
//    これは std::function<R (Args...)> の一引数コンストラクタに渡せるオブジェクトと
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_TYPES_INCLUDED_IS_CALLABLE_HPP_
#define ETUDE_TYPES_INCLUDED_IS_CALLABLE_HPP_

#include <utility>
#include <type_traits>
#include "../functional/invoke.hpp"
#include "identity.hpp"

namespace etude {

  template<class R, class... Args>
  struct is_callable_
  {
    template< class T,
      typename = decltype(
        etude::invoke<R>( std::declval<T>(), std::declval<Args>()... )
      )
    >
    static std::true_type test( int );
    
    template<class T>
    static std::false_type test( ... );
    
  };
  
  template<class T, class Singature>
  class is_callable;
  
  template<class T, class R, class... Args>
  struct is_callable<T, R (Args...)> :
    etude::identity<
      decltype( is_callable_<R, Args...>::template test<T>(0) )
    >::type
  {
  };

} // namespace etude

#endif  // #ifndef ETUDE_TYPES_INCLUDED_IS_CALLABLE_HPP_
