//
//  tuple_element :
//    std::tuple_element の改良版
//    
//    etude::tuple_element<I, Tuple>::type は、
//    Tuple が参照でないの場合は std::tuple_element<I, Tuple>::type と等しく、
//    Tuple が参照の場合は std::get<I>( std::declval<Tuple>() ) の戻り値の型になります。
//    N3225 で rvalue に対する get が定義されたため、
//    それも正しく扱える（ rvalue-ref ならば rvalue-ref になる）ようになってます。
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_TYPES_INCLUDED_TUPLE_ELEMENT_HPP_
#define ETUDE_TYPES_INCLUDED_TUPLE_ELEMENT_HPP_

#include <tuple>

namespace etude {

  // デフォルトの場合
  template<std::size_t I, class Tuple>
  struct tuple_element
    : std::tuple_element<I, Tuple> {};
  
  #define ETUDE_TUPLE_ELEMENT_GEN_( qualifier )                       \
    template<std::size_t I, class Tuple>                              \
    struct tuple_element<I, Tuple qualifier> {                        \
      typedef typename tuple_element<I, Tuple>::type qualifier type;  \
    }                                                                 \
    /* ETUDE_TUPLE_ELEMENT_GEN_( qualifier ) */
    
    ETUDE_TUPLE_ELEMENT_GEN_( const );
    ETUDE_TUPLE_ELEMENT_GEN_( volatile );
    ETUDE_TUPLE_ELEMENT_GEN_( const volatile );
    ETUDE_TUPLE_ELEMENT_GEN_( & );
    ETUDE_TUPLE_ELEMENT_GEN_( && );
    
  #undef ETUDE_TUPLE_ELEMENT_GEN_

} // namespace etude

#endif  // #ifndef ETUDE_TYPES_INCLUDED_TUPLE_ELEMENT_HPP_
