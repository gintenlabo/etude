//
//  etude/immovable.hpp �ɑ΂���e�X�g
//    �Ƃ������d�l
//
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#include "../etude/immovable.hpp"

#include <type_traits>
#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

// gcc 4.5.0 �ɂȂ��̂Ŏ��삷��
template<class T>
struct is_copy_constructible
  : std::is_constructible<T, T const&>::type {};

template<class T>
struct is_move_constructible
  : std::is_constructible<T, T&&>::type {};

#include "../etude/types/is_assignable.hpp"

template<class T>
struct is_copy_assignable
  : etude::is_assignable<T, T const&>::type {};

template<class T>
struct is_move_assignable
  : etude::is_assignable<T, T&&>::type {};


int main()
{
  // etude::immovable<> �ɑ΂���
  STATIC_ASSERT(( !is_copy_constructible<etude::immovable<>>::value ));
  STATIC_ASSERT(( !is_move_constructible<etude::immovable<>>::value ));
  
  STATIC_ASSERT(( !is_copy_assignable<etude::immovable<>>::value ));
  STATIC_ASSERT(( !is_move_assignable<etude::immovable<>>::value ));
  
  STATIC_ASSERT(( std::is_empty<etude::immovable<>>::value ));
  
  STATIC_ASSERT(( std::has_trivial_default_constructor<etude::immovable<>>::value ));
  STATIC_ASSERT(( std::has_trivial_destructor<etude::immovable<>>::value ));


  // base class chaining
  struct X {};
  
  STATIC_ASSERT(( std::is_base_of<X, etude::immovable<X>>::value ));

  STATIC_ASSERT(( !is_copy_constructible<etude::immovable<X>>::value ));
  STATIC_ASSERT(( !is_move_constructible<etude::immovable<X>>::value ));
  
  STATIC_ASSERT(( !is_copy_assignable<etude::immovable<X>>::value ));
  STATIC_ASSERT(( !is_move_assignable<etude::immovable<X>>::value ));
  
  // ctor �]��
  STATIC_ASSERT(( !std::is_convertible<X&&, etude::immovable<X>>::value ));
  STATIC_ASSERT(( std::is_constructible<etude::immovable<X>, X&&>::value ));
  STATIC_ASSERT(( !std::is_constructible<etude::immovable<X>, int>::value ));


  // immovable wrapper
  STATIC_ASSERT(( !is_copy_constructible<etude::immovable<int>>::value ));
  STATIC_ASSERT(( !is_move_constructible<etude::immovable<int>>::value ));
  
  STATIC_ASSERT(( !is_copy_assignable<etude::immovable<int>>::value ));
  STATIC_ASSERT(( !is_move_assignable<etude::immovable<int>>::value ));
  
  // ctor �]��
  STATIC_ASSERT(( !std::is_convertible<int, etude::immovable<int>>::value ));
  STATIC_ASSERT(( std::is_constructible<etude::immovable<int>, int>::value ));
  STATIC_ASSERT(( !std::is_constructible<etude::immovable<int>, void*>::value ));
}
