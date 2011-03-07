//
//  value_holder のテストです。
//    
//    殆ど holder の焼き直しですが
//    
//  Copyright (C) 2010-11  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/utility/value_holder.hpp"
#include "../../etude/unpack.hpp"
#include <type_traits>
#include <utility>
#include <boost/assert.hpp>

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

// チェック本体
template<class T>
void check()
{
  typedef etude::value_holder<T> holder;
  typedef typename std::remove_const<T>::type U;
  
  STATIC_ASSERT((  sizeof(T) ==  sizeof(holder) || std::is_reference<T>::value ));
  STATIC_ASSERT(( alignof(T) == alignof(holder) || std::is_reference<T>::value ));
  STATIC_ASSERT(( std::is_empty<T>::value == std::is_empty<holder>::value ));
  // STATIC_ASSERT(( std::is_trivially_copyable<T>::value == std::is_trivially_copyable<holder>::value )); // ない
  STATIC_ASSERT((
    ( std::is_reference<T>::value && std::is_standard_layout<holder>::value ) ||
    std::is_standard_layout<T>::value == std::is_standard_layout<holder>::value
  ));
  // trivially destructible class か
  STATIC_ASSERT((
    ( std::is_reference<T>::value && std::has_trivial_destructor<holder>::value ) ||
    std::has_trivial_destructor<T>::value == std::has_trivial_destructor<holder>::value
  ));
  
  // get
  STATIC_ASSERT(( std::is_same<
    T &, decltype( std::declval<holder&>().get() )
  >::value ));
  STATIC_ASSERT(( std::is_same<
    T const&, decltype( std::declval<holder const&>().get() )
  >::value ));
  STATIC_ASSERT(( std::is_same<
    U &&, decltype( std::declval<holder&>().move() )
  >::value ));
  
  STATIC_ASSERT(( std::is_same<
    T &, decltype( get( std::declval<holder&>() ) )
  >::value ));
  STATIC_ASSERT(( std::is_same<
    T const&, decltype( get( std::declval<holder const&>() ) )
  >::value ));
  STATIC_ASSERT(( std::is_same<
    U &&, decltype( get( std::declval<holder&&>() ) )
  >::value ));
  
  // operator*
  STATIC_ASSERT(( std::is_same<
    T &, decltype( *std::declval<holder&>() )
  >::value ));
  STATIC_ASSERT(( std::is_same<
    T const&, decltype( *std::declval<holder const&>() )
  >::value ));
  STATIC_ASSERT(( std::is_same<
    U &&, decltype( *std::declval<holder&&>() )
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
  typedef etude::value_holder<T> holder;
  STATIC_ASSERT(( std::is_convertible<U, T>::value
    == std::is_convertible<U, holder>::value ));
  // 型変換できるか否か
  STATIC_ASSERT(( std::is_convertible<U, T>::value
    == std::is_convertible<etude::value_holder<U>, holder>::value ));
}
template<class T, class... Args,
  class = typename std::enable_if<sizeof...(Args) != 1>::type
>
void check_convertible( ... ) {}

// 構築可能性
template<class T, class... Args>
void check_constructible()
{
  typedef etude::value_holder<T> holder;
  
  static bool const is_constructible = std::is_constructible<T, Args...>::value;
  
  STATIC_ASSERT((
    std::is_constructible<holder, etude::emplace_construct_t, Args...>::value
      == is_constructible
  ));
  
  STATIC_ASSERT((
    std::is_constructible<holder, etude::unpack_construct_t, std::tuple<Args...>>::value
      == is_constructible
  ));
  
  STATIC_ASSERT((
    std::is_constructible<holder,
      decltype(
        etude::unpack( std::declval<std::tuple<Args...>>() )
      )
    >::value
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

#include "../../etude/utility/forward_as_tuple.hpp"

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
  
  // value_holder<int*> に 0 を渡したりとか
  etude::value_holder<int*> p = 0;
  BOOST_ASSERT( p.get() == 0 );
  
  // 再代入
  struct X
  {
    X& operator=( int ) {
      return *this;
    }
  };
  etude::value_holder<X const> x;
  x = 43;
  
  // 実際にどのコンストラクタが呼ばれているか
  
  // direct initialization
  etude::value_holder<nontrivial_class> a( etude::emplace_construct, 1 );
  BOOST_ASSERT( a.get().which_ctor_has_called == nontrivial_class::from_int );
  
  // copy construction
  etude::value_holder<nontrivial_class> b( get(a) );
  BOOST_ASSERT( b.get().which_ctor_has_called == nontrivial_class::copy_ctor );
  
  etude::value_holder<nontrivial_class> c( get( std::move(a) ) );
  BOOST_ASSERT( c.get().which_ctor_has_called == nontrivial_class::move_ctor );
  
  // unpack construction
  etude::value_holder<nontrivial_class> d( etude::unpack_construct,
    etude::forward_as_tuple( get(a) )
  );
  BOOST_ASSERT( d.get().which_ctor_has_called == nontrivial_class::copy_ctor );
  
  etude::value_holder<nontrivial_class> e(
    etude::unpack( etude::forward_as_tuple( get( std::move(c) ) ) )
  );
  BOOST_ASSERT( e.get().which_ctor_has_called == nontrivial_class::move_ctor );
}
