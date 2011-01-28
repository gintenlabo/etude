//
//  cast のテストです。
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/utility/cast.hpp"

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

template<class T, class U>
inline void check_type( U && ) {
  STATIC_ASSERT(( std::is_same<T&&, U&&>::value ));
}

struct is_castable_
{
  template< class From, class To,
    class = decltype( etude::cast<To>( std::declval<From>() ) )
  >
  static std::true_type test( int );
  
  template< class From, class To >
  static std::false_type test( ... );
  
  template<class From, class To>
  struct apply
  {
    typedef decltype( test<From, To>(0) ) type;
  };
  
};

template<class From, class To>
struct is_castable
  : is_castable_::template apply<From, To>::type {};

template<class T, class U>
void check_uncastable( U && ) {
  STATIC_ASSERT(( !is_castable<U, T>::value ));
}

#include <boost/utility/addressof.hpp>
#include <boost/assert.hpp>

template<class T, class U>
void check1( U x )
{
  STATIC_ASSERT(( std::is_convertible<U*, T*>::value ));
  
  U const cx = x;
  
  check_type<T>( etude::cast<T>( x ) );
  check_type<T>( etude::cast<T>( cx ) );
  check_type<T>( etude::cast<T>( std::move(x) ) );
  check_type<T>( etude::cast<T>( std::move(cx) ) );
  
  check_type<T&>( etude::cast<T&>( x ) );
  check_uncastable<T&>( cx );
  check_uncastable<T&>( std::move(x) );
  check_uncastable<T&>( std::move(cx) );
  
  check_type<T const&>( etude::cast<T const&>( x ) );
  check_type<T const&>( etude::cast<T const&>( cx ) );
  check_type<T const&>( etude::cast<T const&>( std::move(x) ) );
  check_type<T const&>( etude::cast<T const&>( std::move(cx) ) );
  
  check_uncastable<T&&>( x );
  check_uncastable<T&&>( cx );
  check_type<T&&>( etude::cast<T&&>( std::move(x) ) );
  check_uncastable<T&&>( std::move(cx) );
  
  // アドレスチェック
  // コピーが行われるのでアドレスは別物になる
  auto address_of = []( T const& x ) -> T const* {
    return boost::addressof(x);
  };
  BOOST_ASSERT( address_of( etude::cast<T>( x ) )            != boost::addressof(x)  );
  BOOST_ASSERT( address_of( etude::cast<T>( cx ) )           != boost::addressof(cx) );
  BOOST_ASSERT( address_of( etude::cast<T>( std::move(x) ) ) != boost::addressof(x)  );
}

template<class T, class U>
void check2( U x )
{
  STATIC_ASSERT(( !std::is_convertible<U*, T*>::value ));
  
  U const cx = x;
  
  check_type<T>( etude::cast<T>( x ) );
  check_type<T>( etude::cast<T>( cx ) );
  check_type<T>( etude::cast<T>( std::move(x) ) );
  check_type<T>( etude::cast<T>( std::move(cx) ) );
  
  check_uncastable<T&>( x );
  check_uncastable<T&>( cx );
  check_uncastable<T&>( std::move(x) );
  check_uncastable<T&>( std::move(cx) );
  
  check_type<T const&>( etude::cast<T const&>( x ) );
  check_type<T const&>( etude::cast<T const&>( cx ) );
  check_type<T const&>( etude::cast<T const&>( std::move(x) ) );
  check_type<T const&>( etude::cast<T const&>( std::move(cx) ) );
  
  check_type<T&&>( etude::cast<T&&>( x ) );
  check_type<T&&>( etude::cast<T&&>( cx ) );
  check_type<T&&>( etude::cast<T&&>( std::move(x) ) );
  check_type<T&&>( etude::cast<T&&>( std::move(cx) ) );
  
  // アドレスチェック
  auto address_of = []( T const& x ) -> void const* {
    return boost::addressof(x);
  };
  BOOST_ASSERT( address_of( etude::cast<T>( x ) )            != boost::addressof(x)  );
  BOOST_ASSERT( address_of( etude::cast<T>( cx ) )           != boost::addressof(cx) );
  BOOST_ASSERT( address_of( etude::cast<T>( std::move(x) ) ) != boost::addressof(x)  );
}

struct Base {};
struct Derived : Base {};

void f(){}
void f(int){}

int main()
{
  // 組み込み型
  int* p = 0;
  check1<int*>( p );
  check2<void*>( p );
  
  // 0 のようなリテラルでも文脈を読んでくれる
  check_type<int*>( etude::cast<int*>( 0 ) );
  
  // ユーザ定義型
  Derived x;
  check1<Base>( x );
  check1<Derived>( x );
  
  // 関数（多重定義されていてもおｋ）
  check_type<void(&)()>( etude::cast<void   ()>( f ) );
  check_type<void(*)()>( etude::cast<void(*)()>( f ) );
  check_type<void(&)()>( etude::cast<void(&)()>( f ) );
  
}
