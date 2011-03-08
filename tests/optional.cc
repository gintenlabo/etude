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

#include <type_traits>
#include <boost/test/minimal.hpp>

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

// etude::optional<int> は trivially destructible
STATIC_ASSERT(( std::has_trivial_destructor< etude::optional<int> >::value ));
// etude::optional<T&> は trivially copyable
STATIC_ASSERT(( std::has_trivial_copy_constructor< etude::optional<int&> >::value ));
STATIC_ASSERT(( std::has_trivial_assign< etude::optional<int&> >::value ));
STATIC_ASSERT(( std::has_trivial_destructor< etude::optional<int&> >::value ));

#include <boost/utility/addressof.hpp>
template<class T>
inline bool is_same_object( T& x, T& y )
{
  return boost::addressof(x) == boost::addressof(y);
}

template<class LHS, class RHS>
inline bool checked_equal( LHS const& lhs, RHS const& rhs )
{
  bool result = ( lhs == rhs );
  BOOST_CHECK( ( lhs != rhs ) == !result );
  return result;
}

template<class LHS, class RHS>
inline bool checked_less( LHS const& lhs, RHS const& rhs )
{
  BOOST_CHECK( ( lhs >  rhs ) ==  ( rhs <  lhs ) );
  BOOST_CHECK( ( rhs >  lhs ) ==  ( lhs <  rhs ) );
  BOOST_CHECK( ( lhs >= rhs ) ==  ( rhs <= lhs ) );
  BOOST_CHECK( ( rhs >= lhs ) ==  ( lhs <= rhs ) );
  
  BOOST_CHECK( ( lhs >= rhs ) == !( lhs <  rhs ) );
  BOOST_CHECK( ( rhs >= lhs ) == !( rhs <  lhs ) );
  BOOST_CHECK( ( lhs <= rhs ) == !( lhs >  rhs ) );
  BOOST_CHECK( ( rhs <= lhs ) == !( rhs >  lhs ) );
  
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
  }
  
  {
    etude::optional<X> x( etude::emplace_construct, 0 );
    BOOST_CHECK( x && x->x == 0 );
    
    // in place
    etude::optional<X const> y( boost::in_place(1) );
    BOOST_CHECK( y && y->x == 1 );
    
    x = boost::in_place<X>(2);
    BOOST_CHECK( x && x->x == 2 );
    
  }
  
  {
    // move のチェック
    etude::optional<Y> x = Y(1);
    BOOST_CHECK( x && x->x == 1 );
    
    etude::optional<Y> y = std::move(x);
    BOOST_CHECK( x->x == 0 && y && y->x == 1 );
    
    x = std::move(y);
    BOOST_CHECK( x->x == 1 && y->x == 0 );
  }
  
  {
    // rebind のチェック
    int i = 0;
    etude::optional<Z> x = Z(i), y;
    y = x;
  }
  
  return 0;
}
