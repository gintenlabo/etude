//
//  ライブラリテスト用の小物をまとめたヘッダ
//    全体的に使用頻度が高そうなものを。
//
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_TEST_INCLUDED_TEST_UTILITIES_HPP_
#define ETUDE_TEST_INCLUDED_TEST_UTILITIES_HPP_

#include <utility>
#include <type_traits>

// copyable 関連

#include "../etude/types/is_copy_constructible.hpp"
#include "../etude/types/is_move_constructible.hpp"
// gcc 4.5.0 にないので自作する
template<class T>
struct is_copy_constructible
  : etude::is_copy_constructible<T>::type {};

template<class T>
struct is_move_constructible
  : etude::is_move_constructible<T>::type {};

#include "../etude/types/is_assignable.hpp"

template<class T>
struct is_copy_assignable
  : etude::is_assignable<T, T const&>::type {};

template<class T>
struct is_move_assignable
  : etude::is_assignable<T, T&&>::type {};

// is_trivially_copyable も無い
#include "../etude/types/bool_constant.hpp"

template<class T>
struct is_trivially_copyable :
  etude::bool_constant<
#if defined(__GNUC__)
    std::has_trivial_destructor<T>::value &&
    std::has_trivial_copy_constructor<T>::value &&
  #if ( __GNUC__ == 4 ) && ( __GNUC_MINOR__ < 6 )
    std::has_trivial_assign<T>::value
  #else
    std::has_trivial_copy_assign<T>::value
  #endif
#else
    std::is_trivially_destructible<T>::value &&
    std::is_trivially_copy_constructible<T>::value &&
    std::is_trivially_copy_assignable<T>::value
#endif
  >::type
{};

#endif  // #ifndef ETUDE_TEST_INCLUDED_TEST_UTILITIES_HPP_
