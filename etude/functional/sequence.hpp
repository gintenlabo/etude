//
//  sequence:
//    順に関数を呼ぶファンクタ
// 
//    関数合成…とはちょっと違うか。 与えられた引数 as... に対して、順に
//    f1( as... ); f2( as... ); ... ; fn( as... ); を呼んで void を返す関数を作ります。
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_FUNCTINAL_INCLUDED_SEQUENCE_HPP_
#define ETUDE_FUNCTINAL_INCLUDED_SEQUENCE_HPP_

#include <tuple>
#include <type_traits>
#include <utility>
#include <boost/preprocessor/empty.hpp>
#include "../types/is_convertible.hpp"
#include "../types/decay_and_strip.hpp"

namespace etude {
  
  template<class... Fs>
  struct function_sequence
  {
    template<class... Fs_,
      class = typename std::enable_if<
        etude::is_convertible<types<Fs_&&...>, types<Fs...>>::value
      >::type
    >
    explicit function_sequence( Fs_&& ...fs_ )
      : fs( std::forward<Fs_>(fs_)... ) {}
    
    function_sequence( function_sequence const& ) = default;
    function_sequence( function_sequence && ) = default;
    
    typedef void result_type;
    
    #define ETUDE_SEQUENCE_GEN_( cv )                     \
      template<class... Args>                             \
      void operator()( Args&& ...args ) cv {              \
        return apply_<0>( std::forward<Args>(args)... );  \
      }                                                   \
      /* ETUDE_SEQUENCE_GEN_( cv ) */
      
      ETUDE_SEQUENCE_GEN_( BOOST_PP_EMPTY() )
      ETUDE_SEQUENCE_GEN_( const )
      
    #undef ETUDE_SEQUENCE_GEN_
    
   private:
    std::tuple<Fs...> fs;
    
    // I .. N-1 まで連続して関数適用する
    #define ETUDE_SEQUENCE_GEN_( cv )                                 \
      template<std::size_t I, class... Args,                          \
        class = typename std::enable_if<( I < sizeof...(Fs) )>::type  \
      >                                                               \
      void apply_( Args&& ...args ) cv {                              \
        std::get<I>(fs)( static_cast<Args const&>(args)... );         \
        return apply_<I+1>( std::forward<Args>(args)... );            \
      }                                                               \
      /* ETUDE_SEQUENCE_GEN_( cv ) */
      
      ETUDE_SEQUENCE_GEN_( BOOST_PP_EMPTY() )
      ETUDE_SEQUENCE_GEN_( const )
      
    #undef ETUDE_SEQUENCE_GEN_
    
    // 停止用
    template<std::size_t I, class... Args,
      class = typename std::enable_if<( I == sizeof...(Fs) )>::type
    >
    void apply_( Args const&... ) const { // && だと「オーバーロードできない」エラー。
      // なにもしない
    }
    
  };
  
  template<class... Fs,
    class Seq = function_sequence<typename decay_and_strip<Fs>::type...>
  >
  inline Seq sequence( Fs&& ...fs ) {
    return Seq( std::forward<Fs>(fs)... );
  }
  
} // namespace etude

#endif  // #ifndef ETUDE_FUNCTINAL_INCLUDED_SEQUENCE_HPP_
