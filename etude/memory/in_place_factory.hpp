//
//  in_place_factory:
//    Boost.InPlaceFactory の C++0x 拡張
// 
//    boost::in_place を variadic templates + perfect forward に対応させた版です。
//    普通の apply に加え、rvalue を move させる move_apply が追加されています。
//    本当は move_apply は rvalue references for *this を使って対応すべきなのでしょうが、
//    gcc 4.5.0 にはまだ実装されていないので、泣く泣く専用のメンバを用意しています。
//    ただ、 move 時は専用の関数を使う、というのだと扱いにくいので、
//    統一したアクセスを可能にするために、自由関数 apply_in_place を提供しています。
//    これは boost::in_place_factory でも普通に扱えるので、専らそちらを使うといいかと。
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_MEMORY_INCLUDED_IN_PLACE_FACTORY_
#define ETUDE_MEMORY_INCLUDED_IN_PLACE_FACTORY_

#include <boost/utility/in_place_factory.hpp>

#include <utility>
#include <tuple>
#include <new>
#include <type_traits>

// preprocessor 周り
#include <boost/preprocessor/arithmetic/inc.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/repetition/enum.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/enum_binary_params.hpp>
#include <boost/preprocessor/repetition/repeat_from_to.hpp>
#include <boost/preprocessor/tuple/elem.hpp>

// 設定
#ifndef ETUDE_MAX_ARITY
  #define ETUDE_MAX_ARITY 10
#endif

namespace etude {

  using boost::in_place_factory_base;
  
  // ヘルパメタ関数
  template<class T>
  struct is_in_place_factory
    : std::is_base_of<in_place_factory_base, typename std::remove_reference<T>::type> {};
  
  template<class... Args>
  struct in_place_factory;
  
  template<>
  struct in_place_factory<>
    : in_place_factory_base, private std::tuple<>
  {
    template<class...Args>
    friend class in_place_factory;
    
    typedef std::tuple<> tuple_type;
    
    explicit in_place_factory() {}
    in_place_factory( std::tuple<> ) {}
    
    template<class T>
    T* apply( void* addr ) const {
      return ::new(addr) T();
    }
    template<class T>
    T* move_apply( void* addr ) {
      return ::new(addr) T();
    }
    
    tuple_type const& get_tuple() const {
      return *this;
    }
    tuple_type&& move_tuple() {
      return static_cast<tuple_type&&>(*this);
    }
    
  };
  
  #define ETUDE_GEN_IN_PLACE_( z, n, d )                                      \
    template<BOOST_PP_ENUM_PARAMS(n, class A)>                                \
    struct in_place_factory<BOOST_PP_ENUM_PARAMS(n, A)>                       \
      : in_place_factory_base                                                 \
    {                                                                         \
      typedef std::tuple<BOOST_PP_ENUM_PARAMS(n, A)> tuple_type;              \
                                                                              \
      explicit in_place_factory( BOOST_PP_ENUM_BINARY_PARAMS( n, A, && a ) )  \
        : x( BOOST_PP_ENUM( n, ETUDE_GEN_FORWARD_, _ ) ) {}                   \
                                                                              \
      in_place_factory( tuple_type const& t )                                 \
        : x( t ) {}                                                           \
      in_place_factory( tuple_type && t )                                     \
        : x( std::move(t) ) {}                                                \
      template<class...Args>                                                  \
      in_place_factory( in_place_factory<Args...> const& src )                \
        : x( src.x ) {}                                                       \
      template<class...Args>                                                  \
      in_place_factory( in_place_factory<Args...> && src )                    \
        : x( std::move(src.x) ) {}                                            \
                                                                              \
      template<class T>                                                       \
      T* apply( void* addr ) const {                                          \
        return ::new (addr) T( BOOST_PP_ENUM( n, ETUDE_GEN_GET_X_, _ ) );     \
      }                                                                       \
      template<class T>                                                       \
      T* move_apply( void* addr ) {                                           \
        return ::new (addr) T( BOOST_PP_ENUM( n, ETUDE_GEN_MOVE_X_, _ ) );    \
      }                                                                       \
                                                                              \
      tuple_type const& get_tuple() const { return x; }                       \
      tuple_type&& move_tuple() { return std::move(x); }                      \
                                                                              \
     private:                                                                 \
      tuple_type x;                                                           \
                                                                              \
      template<class...Args>                                                  \
      friend class in_place_factory;                                          \
    };                                                                        \
  /* ETUDE_GEN_IN_PLACE_ */
  
  #define ETUDE_GEN_FORWARD_( z, n, d )   \
    std::forward<BOOST_PP_CAT( A, n )>( BOOST_PP_CAT( a, n ) )
  
  #define ETUDE_GEN_GET_X_( z, n, d )     \
    std::get<n>(x)
    
  #define ETUDE_GEN_MOVE_X_( z, n, d )    \
    std::forward<BOOST_PP_CAT( A, n )>( std::get<n>(x) )
  
  // コード生成
  BOOST_PP_REPEAT_FROM_TO( 1, BOOST_PP_INC(ETUDE_MAX_ARITY), ETUDE_GEN_IN_PLACE_, _ )
  
  #undef ETUDE_GEN_IN_PLACE_
  #undef ETUDE_GEN_FORWARD_
  #undef ETUDE_GEN_GET_X_
  #undef ETUDE_GEN_MOVE_X_
  
  
  // helper functions
  
  // 一時オブジェクトを rvalue-reference として束縛
  // auto を使って束縛されると危険だが、 std::unique_ptr 等に重宝する
  template<class... Args>
  inline in_place_factory<Args&&...> in_place( Args&& ...args ) {
    return in_place_factory<Args&&...>( std::forward<Args&&>(args)... );
  }
  // 一時オブジェクトは値として、それ以外は参照として束縛する
  // こちらは関数の戻り値として使わない限りは auto で束縛しても問題ない。
  // ただし、使い回す場合は、参照なので不意な変更に注意！
  template<class... Args>
  inline in_place_factory<Args...> in_place_by_ref( Args&& ...args ) {
    return in_place_factory<Args...>( std::forward<Args>(args)... );
  }
  
  // タプルを in_place_factory に変換する版。
  // とりあえず詰め込んだ値を使ってオブジェクトを構築したい場合に。
  template<class... Args>
  inline in_place_factory<Args...> in_place_from_tuple( std::tuple<Args...> const& t ) {
    return t;
  }
  template<class... Args>
  inline in_place_factory<Args...> in_place_from_tuple( std::tuple<Args...> && t ) {
    return std::move(t);
  }
  
  // すべて値で束縛する安全版。関数の戻り値としても使える。
  // 参照を束縛したい場合は std::ref を使う。
  template<class... Args>
  inline auto in_place_by_val( Args&& ...args )
    -> decltype( in_place_from_tuple( std::make_tuple( std::forward<Args>(args)... ) ) )
  {
    return in_place_from_tuple( std::make_tuple( std::forward<Args>(args)... ) );
  }
  
  
  // 戻り値と move 対応を盛り込んだヘルパ関数
  
  // boost 対応
  template<class T, class InPlace,
    typename = typename std::enable_if<is_in_place_factory<InPlace>::value>::type>
  inline T* apply_in_place( InPlace const& x, void* addr ) {
    x.template apply<T>( addr );
    return static_cast<T*>( addr );
  }
  
  // etude::in_place_factory 版
  // const 参照
  template<class T, class... Args>
  inline T* apply_in_place( in_place_factory<Args...> const& x, void* addr ) {
    return x.template apply<T>( addr );
  }
  // move
  template<class T, class... Args>
  inline T* apply_in_place( in_place_factory<Args...> && x, void* addr ) {
    return x.template move_apply<T>( addr );
  }
  
  
  // get_tuple の自由関数版
  // こちらのほうが名前が統一されてるので、基本的にこっちを使うべき
  template<class... Args>
  inline auto get_tuple( in_place_factory<Args...> const& x )
    -> decltype( x.get_tuple() ) { return x.get_tuple(); }
  template<class... Args>
  inline auto get_tuple( in_place_factory<Args...> && x )
    -> decltype( x.move_tuple() ) { return x.move_tuple(); }

}

#endif  // #ifndef ETUDE_MEMORY_INCLUDED_IN_PLACE_FACTORY_
