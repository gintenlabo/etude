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
#include "../types/decay_and_strip.hpp"
#include "../types/is_convertible.hpp"
#include "../utility/holder.hpp"
#include "../utility/compressed_pair.hpp"

namespace etude {
  
  template<class... Fs>
  class function_sequence;
  
  template<>
  struct function_sequence<>
  {
    typedef void result_type;
    
    template<class... Args>
    void operator()( Args&&... ) const {}
    
  };
  
  template<class F0>
  class function_sequence<F0>
    : private etude::holder<F0>
  {
    typedef etude::holder<F0> base;
    using base::get;
    
   public:
    // construct
    template<class F0_,
      class = typename std::enable_if<
        std::is_convertible<F0_, F0>::value
      >::type
    >
    explicit function_sequence( F0_ && f0 )
      : base( std::forward<F0_>(f0) ) {}
    
    function_sequence( function_sequence const& ) = default;
    function_sequence( function_sequence && )     = default;
    
    typedef void result_type;
    
    #define ETUDE_SEQUENCE_GEN_( cv )                         \
      template<class... Args,                                 \
        class = decltype(                                     \
          std::declval<F0 cv&>()( std::declval<Args&&>()... ) \
        )                                                     \
      >                                                       \
      void operator()( Args&&... args ) cv                    \
      {                                                       \
        get()( std::forward<Args>(args)... );                 \
      }                                                       \
      /* ETUDE_SEQUENCE_GEN_( cv ) */
      
      ETUDE_SEQUENCE_GEN_( BOOST_PP_EMPTY() )
      ETUDE_SEQUENCE_GEN_( const )
      
    #undef ETUDE_SEQUENCE_GEN_
    
  };
  
  template<class F0, class... Fs>
  class function_sequence<F0, Fs...>
    : private etude::compressed_pair< F0, function_sequence<Fs...> >
  {
    typedef function_sequence<Fs...> tail;
    typedef etude::compressed_pair< F0, tail > base;
    using base::first; using base::second;
    
   public:
    // construct
    template< class F0_, class... Fs_,
      class = typename std::enable_if<
        etude::is_convertible<etude::types<F0_, Fs_...>, etude::types<F0, Fs...>>::value
      >::type
    >
    explicit function_sequence( F0_ && f0, Fs_&&... fs )
      : base( piecewise_construct, std::tuple<F0_&&>( std::forward<F0_>(f0) ),
              std::tuple<Fs_&&...>( std::forward<Fs_>(fs)... ) ) {}
    
    function_sequence( function_sequence const& ) = default;
    function_sequence( function_sequence && )     = default;
    
    typedef void result_type;
    
    #define ETUDE_SEQUENCE_GEN_( cv )                               \
      template<class... Args,                                       \
        class = decltype (                                          \
          std::declval<F0 cv&>()( std::declval<Args const&>()... )  \
        ),                                                          \
        class = decltype (                                          \
          std::declval<tail cv&>()( std::declval<Args&&>()... )     \
        )                                                           \
      >                                                             \
      void operator()( Args&&... args ) cv                          \
      {                                                             \
        first()( std::forward<Args const&>(args)... );              \
        return second()( std::forward<Args>(args)... );             \
      }                                                             \
      /* ETUDE_SEQUENCE_GEN_( cv ) */
      
      ETUDE_SEQUENCE_GEN_( BOOST_PP_EMPTY() )
      ETUDE_SEQUENCE_GEN_( const )
      
    #undef ETUDE_SEQUENCE_GEN_
    
  };
  
  template<class... Fs,
    class Seq = function_sequence<typename decay_and_strip<Fs>::type...>
  >
  inline Seq sequence( Fs&& ...fs ) {
    return Seq( std::forward<Fs>(fs)... );
  }
  
} // namespace etude

#endif  // #ifndef ETUDE_FUNCTINAL_INCLUDED_SEQUENCE_HPP_
