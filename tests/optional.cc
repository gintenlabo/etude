//
//  optional のテストです。
//    
//    TODO: ちゃんと書く
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../etude/optional.hpp"

template class etude::optional<int>;
template class etude::optional<int const>;
template class etude::optional<int&>;
template class etude::optional<int const&>;
template class etude::optional<int&&>;

#include <type_traits>
#include <boost/test/minimal.hpp>

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )
#include "test_utilities.hpp"

// etude::optional<int> は trivially destructible
STATIC_ASSERT(( std::has_trivial_destructor< etude::optional<int> >::value ));
// etude::optional<T&> は trivially copyable かつ standard layout
STATIC_ASSERT(( std::has_trivial_copy_constructor< etude::optional<int&> >::value ));
STATIC_ASSERT(( std::has_trivial_assign< etude::optional<int&> >::value ));
STATIC_ASSERT(( std::has_trivial_destructor< etude::optional<int&> >::value ));
STATIC_ASSERT(( std::is_standard_layout< etude::optional<int&> >::value ));

#include <boost/utility/addressof.hpp>
template<class T>
inline bool is_same_object( T const& x, T const& y )
{
  return boost::addressof(x) == boost::addressof(y);
}
// cv 修飾や value category レベルで厳密に同じかどうかをチェック
template<class T>
inline void check_explicitly_same_object( T && x, T && y )
{
  BOOST_CHECK( boost::addressof(x) == boost::addressof(y) );
}
template<class T, class U>
void check_explicitly_same_object( T && x, U && y ) = delete;

// 基本的なチェック
template<class T, class... Args>
inline void basic_check( Args&&... args )
{
  typedef etude::optional<T> optional_t;
  optional_t x( std::forward<Args>(args)... );
  auto const& cx = x;
  
  // typedef
  STATIC_ASSERT(( std::is_same<typename optional_t::element_type, T>::value ));
  STATIC_ASSERT(( !std::is_const<typename optional_t::value_type>::value ));
  
  // 特性
  // optional<T> は standard layout class
  STATIC_ASSERT(( std::is_standard_layout<optional_t>::value ));
  // sizeof( etude::optional<T> ) <= sizeof(T) + alignof(T)
  STATIC_ASSERT ((
    sizeof( optional_t ) <=
    etude::storage_size<T>::value + etude::storage_align<T>::value
  ));
  // T が trivially destructible ならば optional<T> も trivially destructible
  STATIC_ASSERT ((
    std::has_trivial_destructor<optional_t>::value ==
    std::has_trivial_destructor<T>::value
  ));
  // T が参照ならば trivially copyable
  STATIC_ASSERT ((
    !std::is_reference<T>::value || is_trivially_copyable<optional_t>::value
  ));
  // T が copy constructible ならば copy constructible かつ copy assignable
  STATIC_ASSERT ((
    !is_copy_constructible<T>::value ||
    ( is_copy_constructible<optional_t>::value && is_copy_assignable<optional_t>::value )
  ));
  // T が move constructible ならば move constructible かつ move assignable
  STATIC_ASSERT ((
    !is_move_constructible<T>::value ||
    ( is_move_constructible<optional_t>::value && is_move_assignable<optional_t>::value )
  ));
  
  
  // 共通部分はデフォルト構築と要素取得くらいしか無いので、そこのみチェック
  
  // 構築
  {
    // デフォルト構築
    optional_t x0;
    BOOST_CHECK( !x0 );
    
    // デフォルト構築された optional の不等号チェック
    BOOST_CHECK(    x0 == boost::none   );
    BOOST_CHECK( !( x0 != boost::none ) );
    BOOST_CHECK( !( x0 <  boost::none ) );
    BOOST_CHECK( !( x0 >  boost::none ) );
    BOOST_CHECK(    x0 <= boost::none   );
    BOOST_CHECK(    x0 >= boost::none   );
    
    BOOST_CHECK(    boost::none == x0   );
    BOOST_CHECK( !( boost::none != x0 ) );
    BOOST_CHECK( !( boost::none <  x0 ) );
    BOOST_CHECK( !( boost::none >  x0 ) );
    BOOST_CHECK(    boost::none <= x0   );
    BOOST_CHECK(    boost::none >= x0   );
    
  }
  
  // 中身取得
  auto const p = x.get_ptr();
  STATIC_ASSERT ((
    std::is_same<decltype(p), typename optional_t::pointer const>::value
  ));
  BOOST_CHECK( p == get_pointer(x) );
  BOOST_CHECK( p == cx.get_ptr() );
  BOOST_CHECK( p == get_pointer(cx) );
  
  if( p ) {
    BOOST_CHECK( x != boost::none );
    
    // 値の審査
    check_explicitly_same_object( *p, *x );
    BOOST_CHECK( p == x.operator->() );
    check_explicitly_same_object( *p, x.get() );
    
    BOOST_CHECK( is_same_object( *p, *cx ) );
    BOOST_CHECK( p == cx.operator->() );
    check_explicitly_same_object( *cx, cx.get() );
    
    auto && rv = x.move();
    BOOST_CHECK( is_same_object( *p, rv ) );
    check_explicitly_same_object( *std::move(x), x.move() );
    
    // 型の審査
    STATIC_ASSERT((
      std::is_same<decltype( *x ), T&>::value
    ));
    STATIC_ASSERT((
      std::is_same<decltype( *cx ), T const&>::value
    ));
    STATIC_ASSERT((
      std::is_same<decltype( *std::move(x) ),
        typename std::remove_const<T>::type&&>::value
    ));
    STATIC_ASSERT((
      std::is_same<decltype( x.get() ), T&>::value
    ));
    STATIC_ASSERT((
      std::is_same<decltype( cx.get() ), T const&>::value
    ));
    STATIC_ASSERT((
      std::is_same<decltype( x.move() ),
        typename std::remove_const<T>::type&&>::value
    ));
    STATIC_ASSERT((
      std::is_same<decltype( x.get_ptr() ),
        typename optional_t::pointer>::value
    ));
    STATIC_ASSERT((
      std::is_same<decltype( cx.get_ptr() ),
        typename optional_t::const_pointer>::value
    ));
  }
}

template<class LHS, class RHS>
inline bool checked_equal( LHS const& lhs, RHS const& rhs )
{
  bool result = ( lhs == rhs );
  BOOST_CHECK( ( rhs == lhs ) ==  result );
  BOOST_CHECK( ( lhs != rhs ) == !result );
  BOOST_CHECK( ( rhs != lhs ) == !result );
  return result;
}

template<class LHS, class RHS>
inline bool checked_less( LHS const& lhs, RHS const& rhs )
{
  BOOST_CHECK( ( lhs >  rhs ) == ( rhs <  lhs ) );
  BOOST_CHECK( ( rhs >  lhs ) == ( lhs <  rhs ) );
  BOOST_CHECK( ( lhs >= rhs ) == ( rhs <= lhs ) );
  BOOST_CHECK( ( rhs >= lhs ) == ( lhs <= rhs ) );
  
  BOOST_CHECK( ( lhs >= rhs ) == !( lhs < rhs ) );
  BOOST_CHECK( ( rhs >= lhs ) == !( rhs < lhs ) );
  BOOST_CHECK( ( lhs <= rhs ) == !( lhs > rhs ) );
  BOOST_CHECK( ( rhs <= lhs ) == !( rhs > lhs ) );
  
  return lhs < rhs;
}

void check_compare()
{
  etude::optional<int> x0, y0, x1 = 1, y1 = 1, x2 = 2;
  
  BOOST_CHECK(  checked_equal( x0, x0 ) );
  BOOST_CHECK( !checked_less ( x0, x0 ) );
  
  BOOST_CHECK(  checked_equal( x0, y0 ) );
  BOOST_CHECK( !checked_less ( x0, y0 ) );
  
  BOOST_CHECK(  checked_equal( x0, boost::none ) );
  BOOST_CHECK( !checked_less ( x0, boost::none ) );
  
  BOOST_CHECK( !checked_equal( x0, 1 ) );
  BOOST_CHECK(  checked_less ( x0, 1 ) );
  BOOST_CHECK( !checked_less ( 1, x0 ) );
  
  BOOST_CHECK( !checked_equal( x0, x1 ) );
  BOOST_CHECK(  checked_less ( x0, x1 ) );
  BOOST_CHECK( !checked_less ( x1, x0 ) );
  
  BOOST_CHECK(  checked_equal( x1, x1 ) );
  BOOST_CHECK( !checked_less ( x1, x1 ) );
  
  BOOST_CHECK(  checked_equal( x1, y1 ) );
  BOOST_CHECK( !checked_less ( x1, y1 ) );
  
  BOOST_CHECK( !checked_equal( x1, x2 ) );
  BOOST_CHECK(  checked_less ( x1, x2 ) );
  BOOST_CHECK( !checked_less ( x2, x1 ) );
  
  BOOST_CHECK( !checked_equal( x1, boost::none ) );
  BOOST_CHECK( !checked_less ( x1, boost::none ) );
  BOOST_CHECK(  checked_less ( boost::none, x1 ) );
  
  BOOST_CHECK(  checked_equal( x1, 1 ) );
  BOOST_CHECK( !checked_less ( x1, 1 ) );
  BOOST_CHECK( !checked_less ( 1, x1 ) );
  
  BOOST_CHECK( !checked_equal( x1, 2 ) );
  BOOST_CHECK(  checked_less ( x1, 2 ) );
  BOOST_CHECK( !checked_less ( 2, x1 ) );
  
  BOOST_CHECK( !checked_equal( x1, -1 ) );
  BOOST_CHECK( !checked_less ( x1, -1 ) );
  BOOST_CHECK(  checked_less ( -1, x1 ) );
}

#include "../etude/immovable.hpp"

// non-copyable class
struct X
  : private etude::immovable<>
{
  int x;
  
  explicit X( int x_ )
    : x( x_ ) {}
};

// move のみ可能な class
struct Y
  : private etude::immovable<>
{
  int x;
  
  explicit Y( int x_ )
    : x( x_ ) {}
  
  Y( Y && src )
    : x( src.x )
  {
    src.x = 0;
  }
  
  Y& operator=( Y && rhs )
  {
    Y temp = std::move(rhs);
    this->x = temp.x;
    temp.x = 0;
    return *this;
  }
  
};

// rebind 不能なクラス
struct Z
{
  int& x;
  
  explicit Z( int& x_ )
    : x( x_ ) {}
  
  // 明示的に delete しないと gcc4.5.0 ではダメなよう。
  void operator=( Z const& ) = delete;
  
};

#include <boost/none.hpp>
int test_main( int, char** )
{
  {
    etude::optional<int> x;
    basic_check<int>( x );
    
    BOOST_CHECK( x == boost::none );
    // 自己代入チェック
    x = x;
    BOOST_CHECK( x == boost::none );
    
    int i = 0;
    x = etude::make_optional(i);
    BOOST_CHECK( x && *x == 0 && !is_same_object( *x, i ) );
    // 自己代入チェック
    x = x;
    BOOST_CHECK( x && *x == 0 );
    x = *x;
    BOOST_CHECK( x && *x == 0 );
    
    x = 1;
    etude::optional<int> y = x;
    x = boost::none;
    swap( x, y );
    BOOST_CHECK( x && x == 1 && !y );
    
    check_compare();
    
    etude::optional<int const> z = 1;
    z = 0;  // 問題なし
    // *z = 1; // ダメ
    basic_check<int const>( z );
    basic_check<int&>( i );
  }
  
  {
    etude::optional<X> x( etude::emplace_construct, 0 );
    BOOST_CHECK( x && x->x == 0 );
    
    // in place
    etude::optional<X const> y( boost::in_place(1) );
    BOOST_CHECK( y && y->x == 1 );
    
    x = boost::in_place<X>(2);
    BOOST_CHECK( x && x->x == 2 );
    
    basic_check<X>( etude::emplace_construct, 0 );
  }
  
  {
    // move のチェック
    etude::optional<Y> x = Y(1);
    BOOST_CHECK( x && x->x == 1 );
    
    etude::optional<Y> y = std::move(x);
    BOOST_CHECK( x->x == 0 && y && y->x == 1 );
    
    x = std::move(y);
    BOOST_CHECK( x->x == 1 && y->x == 0 );
    
    basic_check<Y>( Y(0) );
    basic_check<Y const>( Y(0) );
  }
  
  {
    // rebind のチェック
    int i = 0;
    etude::optional<Z> x = Z(i), y;
    y = x;
    // 自己代入
    x = x;
    BOOST_CHECK( is_same_object( x->x, i ) );
    x = *x;
    BOOST_CHECK( is_same_object( x->x, i ) );
    
    basic_check<Z>( x );
  }
  
  return 0;
}
