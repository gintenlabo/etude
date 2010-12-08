//
//  etude::is_convertible のテストです。
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/types/is_convertible.hpp"

#include <type_traits>

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

template<class T>
struct is_etude_types
  : std::false_type {};

template<class... Types>
struct is_etude_types<etude::types<Types...>>
  : std::true_type {};

template<class From, class To>
struct checked_is_convertible
  : etude::is_convertible<From, To>
{
  static bool const std_conv = std::is_convertible<From, To>::value,
    etude_conv = etude::is_convertible<From, To>::value;
  // std::is_convertible => etude::is_convertible
  // 逆は成り立たない
  // かつ std::is_convertible != etude::is_convertible ならば
  // From, To の両方が etude::types である
  STATIC_ASSERT((
    !( std_conv != etude_conv ) || (
      etude_conv == true && (
        is_etude_types<From>::value && is_etude_types<To>::value
      )
    )
  ));
};

int main()
{
  // int -> int 。 おｋ
  STATIC_ASSERT(( checked_is_convertible<int, int>::value ));
  
  // types に包まれた int -> int 。 これは std:: でもおｋ
  STATIC_ASSERT(( checked_is_convertible<etude::types<int>, etude::types<int>>::value ));
  STATIC_ASSERT(( std::is_convertible<etude::types<int>, etude::types<int>>::value ));
  // types に包まれた int -> double 。 これは std:: ではダメ
  STATIC_ASSERT(( checked_is_convertible<etude::types<int>, etude::types<double>>::value ));
  STATIC_ASSERT(( !std::is_convertible<etude::types<int>, etude::types<double>>::value ));
  // types に包まれた int -> char* 。 これは両方ダメ
  STATIC_ASSERT(( !checked_is_convertible<etude::types<int>, etude::types<char*>>::value ));
  STATIC_ASSERT(( !std::is_convertible<etude::types<int>, etude::types<char*>>::value ));

  // 参照の場合は etude の場合でも中身まで調べない
  STATIC_ASSERT(( !checked_is_convertible<etude::types<int>&, etude::types<double>>::value ));
  STATIC_ASSERT(( !std::is_convertible<etude::types<int>&, etude::types<double>>::value ));
  
  // 複数の型のチェック
  STATIC_ASSERT(( 
    checked_is_convertible<etude::types<int&, double>, etude::types<int&, double>>::value
  ));
  STATIC_ASSERT(( 
    checked_is_convertible<etude::types<int&, float>, etude::types<int const&, double>>::value
  ));
  STATIC_ASSERT(( 
    !checked_is_convertible<etude::types<int&, float>, etude::types<char*, float>>::value
  ));
  STATIC_ASSERT(( 
    !checked_is_convertible<etude::types<int&, char*>, etude::types<int&, float>>::value
  ));
  // 数違い
  STATIC_ASSERT(( 
    !checked_is_convertible<
      etude::types<int&, int&, int&>, etude::types<int&>>::value
  ));
  STATIC_ASSERT(( 
    !checked_is_convertible<
      etude::types<>, etude::types<int&, void, char*>>::value
  ));
;
}
