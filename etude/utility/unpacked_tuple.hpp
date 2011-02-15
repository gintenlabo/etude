//
//  unpacked_tuple:
//    タプルからの値の get
// 
//    ・現行の gcc が && 版の std::get に対応していないので、それに対応
//    ・直接 std::get って呼ぶと std::get が dependent name にならないので
//      std::array 等を後から include した場合に std::get が見つからない問題にも対処
//  
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_UTILITY_INCLUDED_UNPACKED_TUPLE_HPP_
#define ETUDE_UTILITY_INCLUDED_UNPACKED_TUPLE_HPP_

#include "../types/tuple_element.hpp"
#include "../utility/tuple_get.hpp"

#include <utility>
#include <type_traits>

namespace etude {

  template<class Tuple, std::size_t... Indices>
  struct unpacked_tuple
  {
    typedef Tuple tuple_type;
    
    unpacked_tuple() = default;
    
    unpacked_tuple( unpacked_tuple const& ) = default;
    unpacked_tuple( unpacked_tuple && ) = default;
    
    unpacked_tuple( Tuple && t )
      : t_( std::forward<Tuple>(t) ) {}
    
    template< class U,
      class = typename std::enable_if<
        std::is_convertible<U, Tuple>::value
      >::type
    >
    unpacked_tuple( U && x )
      : t_( std::forward<U>(x) ) {}
    
    template< class... Args,
      class = typename std::enable_if<
        std::is_constructible<Tuple, Args...>::value
      >::type
    >
    explicit unpacked_tuple( Args&&... args )
      : t_( std::forward<Args>(args)... ) {}
    
    // tuple の取得
    Tuple      & get_tuple()       { return t_; }
    Tuple const& get_tuple() const { return t_; }
    Tuple &&    move_tuple()       { return std::forward<Tuple>(t_); }
    
   private:
    Tuple t_;
    
  };
  
  // 要素アクセス
  template<std::size_t N, class Tuple, std::size_t... Indices>
  inline typename etude::tuple_element<N, Tuple&>::type
    get( unpacked_tuple<Tuple, Indices...> & x )
  {
    return etude::tuple_get<N>( x.get_tuple() );
  }
  template<std::size_t N, class Tuple, std::size_t... Indices>
  inline typename etude::tuple_element<N, Tuple const&>::type
    get( unpacked_tuple<Tuple, Indices...> const& x )
  {
    return etude::tuple_get<N>( x.get_tuple() );
  }
  template<std::size_t N, class Tuple, std::size_t... Indices>
  inline typename etude::tuple_element<N, Tuple&&>::type
    get( unpacked_tuple<Tuple, Indices...> && x )
  {
    return etude::tuple_get<N>( x.move_tuple() );
  }
  // 中身を move
  template<std::size_t N, class Tuple, std::size_t... Indices>
  inline typename etude::tuple_element<N, Tuple&&>::type
    move( unpacked_tuple<Tuple, Indices...> & x )
  {
    return etude::tuple_forward<N, Tuple>( x.get_tuple() );
  }
  template<std::size_t N, class Tuple, std::size_t... Indices>
  inline typename etude::tuple_element<N, Tuple&&>::type
    move( unpacked_tuple<Tuple, Indices...> && x )
  {
    return etude::tuple_forward<N, Tuple>( x.get_tuple() );
  }

} // namespace etude

#endif  // #ifndef ETUDE_UTILITY_INCLUDED_UNPACKED_TUPLE_HPP_
