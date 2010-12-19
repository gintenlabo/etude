//
//  value_wrapper のテストです。
//    
//    殆ど simple_wrapper の焼き直しですが
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/utility/value_wrapper.hpp"
#include <type_traits>
#include <utility>

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

// チェック本体
template<class T>
void check()
{
  typedef etude::value_wrapper<T> wrapper;
  
  STATIC_ASSERT((  sizeof(T) ==  sizeof(wrapper) || std::is_reference<T>::value ));
  STATIC_ASSERT(( alignof(T) == alignof(wrapper) || std::is_reference<T>::value ));
  STATIC_ASSERT(( std::is_empty<T>::value == std::is_empty<wrapper>::value ));
  // STATIC_ASSERT(( std::is_trivially_copyable<T>::value == std::is_trivially_copyable<wrapper>::value )); // ない
  STATIC_ASSERT(( std::is_standard_layout<T>::value
    == std::is_standard_layout<wrapper>::value ));
  // trivially destructible class か
  STATIC_ASSERT(( std::has_trivial_destructor<T>::value
    == std::has_trivial_destructor<wrapper>::value ));
  
  // get
  STATIC_ASSERT(( std::is_same<
    T&, decltype( std::declval<wrapper&>().get() )
  >::value ));
  STATIC_ASSERT(( std::is_same<
    T const&, decltype( std::declval<wrapper const&>().get() )
  >::value ));
  
  STATIC_ASSERT(( std::is_same<
    T &, decltype( get( std::declval<wrapper&>() ) )
  >::value ));
  STATIC_ASSERT(( std::is_same<
    T const&, decltype( get( std::declval<wrapper const&>() ) )
  >::value ));
  STATIC_ASSERT(( std::is_same<
    T &&, decltype( get( std::declval<wrapper&&>() ) )
  >::value ));
}

// （CVつきの）値と参照についてチェック
template<class T>
void test()
{
  check<T>();
  check<T const>();
  check<T &>();
  check<T const&>();
  check<T &&>();
}

// 変換可能性
template<class T, class U>
void check_convertible( int )
{
  typedef etude::value_wrapper<T> wrapper;
  STATIC_ASSERT(( std::is_convertible<U, T>::value
    == std::is_convertible<U, wrapper>::value ));
}
template<class T, class... Args,
  class = typename std::enable_if<sizeof...(Args) != 1>::type
>
void check_convertible( ... ) {}

// 構築可能性
template<class T, class... Args>
void check_constructible()
{
  typedef etude::value_wrapper<T> wrapper;
  
  static bool const is_constructible = std::is_constructible<T, Args...>::value;
  
  STATIC_ASSERT((
    std::is_constructible<wrapper, etude::emplace_construct_t, Args...>::value
      == is_constructible
  ));
  STATIC_ASSERT((
    std::is_constructible<wrapper, etude::unpack_construct_t, std::tuple<Args...>>::value
      == is_constructible
  ));
  
  check_convertible<T, Args...>(0);
}


struct trivial_empty_class {};

struct nontrivial_empty_class {
  nontrivial_empty_class() {}
  ~nontrivial_empty_class() {}
};

struct trivial_class {
  int x, y;
};

struct nontrivial_class
{
  enum {
    copy_ctor,
    move_ctor,
    
    from_int,
    from_intptr,
    from_int_and_double,
    from_int_and_double_and_charptr
    
  } which_ctor_has_called;
  
  nontrivial_class( nontrivial_class const& )
    : which_ctor_has_called(copy_ctor) {}
  nontrivial_class( nontrivial_class && )
    : which_ctor_has_called(move_ctor) {}
  
  // 型変換： int から
  nontrivial_class( int )
    : which_ctor_has_called(from_int) {}
  // int* からは explicit
  explicit nontrivial_class( int* )
    : which_ctor_has_called(from_intptr) {}
  
  // explicit な複数引数コンストラクタ
  explicit nontrivial_class( int, double )
    : which_ctor_has_called(from_int_and_double) {}
  // non-explicit な複数引数コンストラクタ
  nontrivial_class( int, double, char* )
    : which_ctor_has_called(from_int_and_double_and_charptr) {}
  
  // standard layout でない
  virtual ~nontrivial_class() {}
  
};

int main()
{
  test<int>();
  test<trivial_empty_class>();
  test<nontrivial_empty_class>();
  test<trivial_class>();
  test<nontrivial_class>();
  
  check_constructible<int>();
  check_constructible<int, int>();
  check_constructible<int const&, int&>();
  
  check_constructible<trivial_class>();
  check_constructible<nontrivial_class>();
  
  check_constructible<nontrivial_class, int>();
  check_constructible<nontrivial_class, int*>();
  check_constructible<nontrivial_class, void const*>();
  
  check_constructible<nontrivial_class, double, double>();
  check_constructible<nontrivial_class, int*, double>();
  check_constructible<nontrivial_class, int, double, char*>();
  check_constructible<nontrivial_class, int, double, char const*>();
}
