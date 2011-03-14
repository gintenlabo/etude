//
//  optional_reference のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../etude/optional_reference.hpp"
#include "../etude/types/is_equality_comparable.hpp"

#include <type_traits>
#include <boost/test/minimal.hpp>

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

#include <boost/utility/addressof.hpp>
template<class T>
inline bool is_same_object( T& x, T& y )
{
  return boost::addressof(x) == boost::addressof(y);
}

template<class T, class U>
inline void check_comparison (
  etude::optional_reference<T> const& x, etude::optional_reference<U> const& y,
  bool equal_expected, bool less_expected
)
{
  bool const less_or_equal_expected = less_expected || equal_expected;
  
  BOOST_CHECK( ( x == y ) ==  equal_expected );
  BOOST_CHECK( ( x != y ) == !equal_expected );
  BOOST_CHECK( ( x <  y ) ==  less_expected );
  BOOST_CHECK( ( x <= y ) ==  less_or_equal_expected );
  BOOST_CHECK( ( x >  y ) == !less_or_equal_expected );
  BOOST_CHECK( ( x >= y ) == !less_expected );
  
}
struct hoge
{
  int i;
  // アドレスは取れません
  void operator&() const {}
};

#include <boost/none.hpp>
int test_main( int, char** )
{
  // 構築
  {
    // デフォルト構築
    etude::optional_reference<int> const p1, p2 = boost::none;
    BOOST_CHECK( !p1 && !p2 );
    
    // 参照から構築
    int i = 0;
    etude::optional_reference<int> const p3 = i;
    BOOST_CHECK( is_same_object( i, *p3 ) );
    
    etude::optional_reference<int> const p4( true, i );
    etude::optional_reference<int> const p5( false, i );
    BOOST_CHECK( is_same_object( i, *p4 ) && !p5 );
    
    // 他の optional_reference から構築
    etude::optional_reference<int const> const p6 = p1;
    etude::optional_reference<int const> const p7 = p3;
    BOOST_CHECK( p6 == p1 && p7 == p3 );
  }
  
  // 代入と make_optional_reference
  {
    int i = 0;
    etude::optional_reference<int> p = boost::none;
    BOOST_CHECK( p == boost::none );
    
    // 直接代入
    p = i;
    BOOST_CHECK( is_same_object( i, *p ) );
    
    // 参照先の reset
    p = boost::none;
    BOOST_CHECK( !p );
    
    // make_optional_reference によって作った optional_reference を代入
    p = etude::make_optional_reference(i);
    BOOST_CHECK( is_same_object( i, *p ) );
    
    // conditional な make_optional_reference
    p = etude::make_optional_reference( false, i );
    BOOST_CHECK( !p );
    p = etude::make_optional_reference( true, i );
    BOOST_CHECK( is_same_object( i, *p ) );
    
    // make_optional_reference_if
    int* p_ = 0;
    p = etude::make_optional_reference_if( p_ );
    BOOST_CHECK( !p );
    p_ = &i;
    p = etude::make_optional_reference_if( p_ );
    BOOST_CHECK( is_same_object( i, *p ) );
    
    // make_optional_reference_lazy
    auto f = [&]() -> int& { return i; };
    p = etude::make_optional_reference_lazy( false, f );
    BOOST_CHECK( !p );
    p = etude::make_optional_reference_lazy( true, f );
    BOOST_CHECK( is_same_object( i, *p ) );
    
    // swap
    etude::optional_reference<int> p2;
    swap( p, p2 );
    BOOST_CHECK( !p && is_same_object( i, *p2 ) );
  }
  
  // アクセス
  {
    hoge x1, x2;
    BOOST_CHECK( !is_same_object( x1, x2 ) );
    etude::optional_reference<hoge> const p0, p1 = x1;
    
    BOOST_CHECK( p0.get() == 0 );
    BOOST_CHECK( is_same_object( *p1.get(), x1 ) );
    BOOST_CHECK( get(p0) == p0.get() );
    BOOST_CHECK( get(p1) == p1.get() );
    
    BOOST_CHECK( p0.get_ptr() == p0.get() );
    BOOST_CHECK( p1.get_ptr() == p1.get() );
    BOOST_CHECK( get_pointer(p0) == p0.get_ptr() );
    BOOST_CHECK( get_pointer(p1) == p1.get_ptr() );
    
    BOOST_CHECK( is_same_object( *p1, x1 ) );
    BOOST_CHECK( is_same_object( p1->i, (*p1).i ) );
    
    BOOST_CHECK( is_same_object( p0.get_value_or(x2), x2 ) );
    BOOST_CHECK( is_same_object( get_optional_value_or( p0, x2 ), x2 ) );
    BOOST_CHECK( is_same_object( p1.get_value_or(x2), x1 ) );
    BOOST_CHECK( is_same_object( get_optional_value_or( p1, x2 ), x1 ) );
    
    auto f = [&]() -> hoge& { return x2; };
    BOOST_CHECK( is_same_object( p0.get_value_or_invoke(f), x2 ) );
    BOOST_CHECK( is_same_object( get_optional_value_or_invoke( p0, f ), x2 ) );
    BOOST_CHECK( is_same_object( p1.get_value_or_invoke(f), x1 ) );
    BOOST_CHECK( is_same_object( get_optional_value_or_invoke( p1, f ), x1 ) );
  }
  
  // 比較
  {
    int i = 0, j = 0;
    etude::optional_reference<int> p0, p1, p2 = i, p3 = i, p4 = j;
    
    bool const less_0i = std::less<int*>()(  0, &i );
    bool const less_ij = std::less<int*>()( &i, &j );
    
    check_comparison( p0, p0, true, false );  // 同じオブジェクト（NULL）
    check_comparison( p0, p1, true, false );  // 違うオブジェクト（どちらもNULL）
    check_comparison( p0, p2, false, less_0i );  // 違うオブジェクト（左がNULL, 右が非NULL）
    check_comparison( p2, p0, false, !less_0i );  // 違うオブジェクト（左が非NULL, 右がNULL）
    check_comparison( p2, p2, true, false );  // 同じオブジェクト（非NULL）
    check_comparison( p2, p3, true, false );  // 違うオブジェクト（どちらも非NULLの同じ値）
    check_comparison( p2, p4, false, less_ij );  // 違うオブジェクト（非NULLの異なる値）
    
    // 型変換
    etude::optional_reference<int const> p5, p6 = i;
    check_comparison( p0, p5, true, false );  // どちらもNULL
    check_comparison( p0, p6, false, less_0i );  // 左がNULL, 右が非NULL
    check_comparison( p2, p5, false, !less_0i );  // 左が非NULL, 右がNULL
    check_comparison( p2, p6, true, false );  // どちらも非NULLの同じ値
    check_comparison( p4, p6, false, !less_ij );  // 非NULLの異なる値
    
    // 無関係の型は比較できない
    STATIC_ASSERT((
      !etude::is_equality_comparable<
        etude::optional_reference<int>,
        etude::optional_reference<char>
      >::value
    ));
  }
  
  {
    int i = 23;
    etude::optional_reference<int> p0, p1 = i;
    
    BOOST_CHECK( !p0 && p1 );
    
    // optional_reference 越しに変更できてることを確認
    *p1 = 42;
    BOOST_CHECK( i == 42 );
    
    p1 = boost::none;
    BOOST_CHECK( p0 == p1 );
    
    // 配列とかも
    int a[] = { 1, 2, 3, 4, 5 };
    etude::optional_reference<int[5]> const pa = a;
    BOOST_CHECK( (*pa)[3] == 4 );
  }
  
  return 0;
}
