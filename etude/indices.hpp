//
//  indices:
//    インデックスのリストに対する type envelope
// 
//    複数の std::size_t のコンパイル時定数をテンプレート引数に持ったクラスです。
//    主に関数などのテンプレートにインデックスのリストを渡したい場合に、
//    type envelope として使う感じになります。
//    
//    これにより、
//      std::get<Indices>( some_tuple )...
//    のようにして tuple を unpack する、といったことが出来るようになります。
//    
//    この用途に使う場合、 0 .. N-1 のインデックスリストがあると特に便利なので、
//    そのようなリストを生成するメタ関数 make_indices も用意。
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
//  このヘッダは http://www.rhinocerus.net/forum/language-c-moderated/
//    439657-c-0x-tuple-unpacking-arguments-function-call.html#post2038703
//  を参考にしています。
//
#ifndef ETUDE_INCLUDED_INDICES_HPP_
#define ETUDE_INCLUDED_INDICES_HPP_

#include <cstddef>  // for std::size_t

namespace etude {
  
  // simple type-envelope for indices
  template<std::size_t... Indices>
  struct indices {
    typedef indices type;
    static std::size_t const size = sizeof...(Indices);
  };
  
  // make indices<0, 1, ... , N-1>
  
  // implementation classes
  template<std::size_t N, std::size_t... Indices>
  struct make_indices_;
  
  template< bool more_recursion_required, std::size_t N, std::size_t... Indices>
  struct make_indices_impl_
    : make_indices_<N, Indices..., sizeof...(Indices)>
  {
    static_assert( N > sizeof...(Indices), "invalid template argument" );
  };
  template<std::size_t N, std::size_t... Indices>
  struct make_indices_impl_<false, N, Indices...>
  {
    static_assert( N == sizeof...(Indices), "invalid template argument" );
    typedef indices<Indices...> type;
  };
  
  template<std::size_t N, std::size_t... Indices>
  struct make_indices_
    : make_indices_impl_<( N > sizeof...(Indices) ), N, Indices...> {};
  
  // 本体
  template<std::size_t N>
  struct make_indices
    : make_indices_<N>::type {};

} // namespace etude

#endif  // #ifndef ETUDE_INCLUDED_INDICES_HPP_
