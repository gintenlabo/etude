//
//  compressed_pair のテストです。
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/utility/compressed_pair.hpp"
#include <type_traits>
#include <utility>

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

// チェック本体
template<class T1, class T2>
void check()
{
  typedef etude::compressed_pair<T1, T2> pair_type;
  typedef std::pair<T1, T2> std_pair;
  
  STATIC_ASSERT((  sizeof(pair_type) <=  sizeof(std_pair) ));
  STATIC_ASSERT(( alignof(pair_type) == alignof(std_pair) ));
  
  // STATIC_ASSERT((
  //   ( std::is_trivially_copyable<T1>::value && std::is_trivially_copyable<T2>::value )
  //     == std::is_trivially_copyable<pair_type>::value
  // )); // ない
  STATIC_ASSERT((
    ( std::is_standard_layout<etude::holder<T1>>::value && std::is_standard_layout<etude::holder<T2>>::value )
      == std::is_standard_layout<pair_type>::value ));
  // trivially destructible class か
  STATIC_ASSERT((
    ( std::has_trivial_destructor<T1>::value && std::has_trivial_destructor<T2>::value )
      == std::has_trivial_destructor<pair_type>::value ));
  
  // element access
  STATIC_ASSERT(( std::is_same<
    T1&, decltype( std::declval<pair_type&>().first() )
  >::value ));
  STATIC_ASSERT(( std::is_same<
    T2&, decltype( std::declval<pair_type&>().second() )
  >::value ));
  
  STATIC_ASSERT(( std::is_same<
    T1 const&, decltype( std::declval<pair_type const&>().first() )
  >::value ));
  STATIC_ASSERT(( std::is_same<
    T2 const&, decltype( std::declval<pair_type const&>().second() )
  >::value ));
  
  STATIC_ASSERT(( std::is_same<
    typename std::remove_const<T1>::type&&,
    decltype( std::declval<pair_type&>().move_first() )
  >::value ));
  STATIC_ASSERT(( std::is_same<
    typename std::remove_const<T2>::type&&,
    decltype( std::declval<pair_type&>().move_second() )
  >::value ));
  
  STATIC_ASSERT(( std::is_same<
    T1&, decltype( get_first( std::declval<pair_type&>() ) )
  >::value ));
  STATIC_ASSERT(( std::is_same<
    T2&, decltype( get_second( std::declval<pair_type&>() ) )
  >::value ));
  
  STATIC_ASSERT(( std::is_same<
    T1 const&, decltype( get_first( std::declval<pair_type const&>() ) )
  >::value ));
  STATIC_ASSERT(( std::is_same<
    T2 const&, decltype( get_second( std::declval<pair_type const&>() ) )
  >::value ));
  
  STATIC_ASSERT(( std::is_same<
    typename std::remove_const<T1>::type&&,
    decltype( get_first( std::declval<pair_type>() ) )
  >::value ));
  STATIC_ASSERT(( std::is_same<
    typename std::remove_const<T2>::type&&,
    decltype( get_second( std::declval<pair_type>() ) )
  >::value ));
}

// （CVつきの）値と参照についてチェック
template<class T1, class T2>
void test()
{
  check<T1, T2>();
  check<T1 const, T2 const>();
  check<T1 &, T2 const&>();
  check<T1 const&, T2 &&>();
  check<T1 &&, T2 &>();
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

#include <boost/assert.hpp>
#include <tuple>

int main()
{
  test<int, double>();
  test<int, trivial_empty_class>();
  test<nontrivial_empty_class, int>();
  test<trivial_class, nontrivial_class>();
  test<nontrivial_class, trivial_empty_class>();
  test<trivial_empty_class, nontrivial_empty_class>();
  
  
  etude::compressed_pair<int, trivial_empty_class> x;
  STATIC_ASSERT(( sizeof(x) == sizeof(int) ));
  BOOST_ASSERT(( x.first() == 0 ));
  
  typedef etude::compressed_pair<nontrivial_class, nontrivial_class> y_type;
  y_type y( etude::piecewise_construct, std::make_tuple( 0 ), std::make_tuple( 1, 2 ) );
  
  STATIC_ASSERT(( sizeof(y) == sizeof(nontrivial_class) * 2 ));
  BOOST_ASSERT(( y.first().which_ctor_has_called == nontrivial_class::from_int ));
  BOOST_ASSERT(( y.second().which_ctor_has_called == nontrivial_class::from_int_and_double ));
  
  // ポインタや list-initialization も正しく扱えるか
  {
    struct X{ int i; };
    X x = {42};
    void* vp = &x;
    
    etude::compressed_pair<void*, X> p1( 0,  {0} );
    etude::compressed_pair<void*, X> p2( vp, {0} );
    etude::compressed_pair<void*, X> p3( 0,   x  );
    
    // move 版の get_first, get_second
    etude::compressed_pair<X const, X const> p4;
    X&& x1 = get_first( std::move(p4) );
    X&& x2 = get_second( std::move(p4) );
    BOOST_ASSERT( boost::addressof(x1) == boost::addressof(p4.first()) );
    BOOST_ASSERT( boost::addressof(x2) == boost::addressof(p4.second()) );
    
    // 参照の swap
    etude::compressed_pair<X&, X&> p5( p1.second(), p1.second() ),
                                   p6( p2.second(), p3.second() );
    swap( p5, p6 );
    BOOST_ASSERT( boost::addressof(p5.first())  == boost::addressof(p2.second()) );
    BOOST_ASSERT( boost::addressof(p5.second()) == boost::addressof(p3.second()) );
    BOOST_ASSERT( boost::addressof(p6.first())  == boost::addressof(p1.second()) );
    BOOST_ASSERT( boost::addressof(p6.second()) == boost::addressof(p1.second()) );
    
    // uninitialized
    etude::compressed_pair<void*, int> p7 = etude::uninitialized;
    etude::compressed_pair<void*, int> p8( 0, etude::uninitialized );
    etude::compressed_pair<void*, int> p9( etude::uninitialized, 0 );
    etude::compressed_pair<void*, int> p10( etude::uninitialized, etude::uninitialized );
    etude::compressed_pair<void*, int> p11(
      etude::piecewise_construct, 
      std::make_tuple( etude::uninitialized ), std::make_tuple( etude::uninitialized )
    );
  }
  
  // 型変換チェック
  STATIC_ASSERT((
    std::is_constructible<
      y_type, char, nontrivial_class const&
    >::value
  ));
  STATIC_ASSERT((
    !std::is_constructible<
      y_type, int*, nontrivial_class &&
    >::value
  ));
  STATIC_ASSERT((
    std::is_constructible<
      y_type, etude::piecewise_construct_t, std::tuple<int&>, std::tuple<int, int>
    >::value
  ));
  STATIC_ASSERT((
    !std::is_constructible<
      y_type, etude::piecewise_construct_t, std::tuple<int const*>, std::tuple<int, int>
    >::value
  ));
}
