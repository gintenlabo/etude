//
//  is_tuple :
//    tuple なら true_type になるメタ関数
//    
//    is_tuple<T> は、 std::tuple_size<T>::value が存在し 0 以上なら std::true_type を、
//    そうでなければ std::false_type を継承します。
//    
//    具体的には std::tuple, std::pair, std::array が該当します。
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_TYPES_INCLUDED_IS_TUPLE_HPP_
#define ETUDE_TYPES_INCLUDED_IS_TUPLE_HPP_

#include <type_traits>
#include <tuple>

namespace etude {

  // for implementation
  class is_tuple_
  {
    template<class T, class = typename std::enable_if<(
        std::tuple_size<T>::value >= 0
      )>::type
    >
    static std::true_type test(int);
    
    template<class T>
    static std::false_type test(...);
    
   public:
    template<class T>
    struct apply {
      typedef decltype( test<T>(0) ) type;
    };
    
  };
  
  // 本体
  template<class T>
  struct is_tuple
    : is_tuple_::template apply<T>::type {};

} // namespace etude

#endif  // #ifndef ETUDE_TYPES_INCLUDED_IS_TUPLE_HPP_
