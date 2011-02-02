//
//  ptr_fn のテストです。
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#include "../../etude/functional/invoke.hpp"

#include <type_traits>
#include <boost/assert.hpp>
#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

struct Base
{
  Base() : member() {}
  explicit Base( int x ) : member(x) {}
  
  int member;
  
  int foo() { member *= 2; return member; }
  int bar( int i ) const { return i * 2 + member; }
  
};

struct Derived
  : Base
{
  Derived(){}
  explicit Derived( int x ) : Base(x) {}
};

// test1
// INVOKE(f, t1, t2, ..., tN) is (t1.*f)(t2, ..., tN)
// when f is a pointer to a member function of a class T and
// t1 is an object of type T or a reference to an object of type T
// or a reference to an object of a type derived from T
void test1()
{
  auto foo = &Base::foo;
  auto bar = &Base::bar;
  
  // object of type T
  BOOST_ASSERT( etude::invoke( bar, Base(), 2 ) == 4 );
  {
    auto x = etude::invoke<double>( bar, Base(1), 2 );
    STATIC_ASSERT(( std::is_same<double, decltype(x)>::value ));
    BOOST_ASSERT( x == 5 );
  }
  
  // reference to an object of type T
  Base base(1);
  BOOST_ASSERT( etude::invoke( foo, base ) == 2 );
  BOOST_ASSERT( etude::invoke( bar, base, 2 ) == 6 );
  {
    etude::invoke<void>( foo, base );
    BOOST_ASSERT( base.member == 4 );
    
    auto x = etude::invoke<double>( bar, base, 0 );
    STATIC_ASSERT(( std::is_same<double, decltype(x)>::value ));
    BOOST_ASSERT( x == 4 );
  }
  
  // a reference to an object of a type derived from T
  Derived derived(2);
  BOOST_ASSERT( etude::invoke( foo, derived ) == 4 );
  BOOST_ASSERT( etude::invoke( bar, derived, 2 ) == 8 );
  {
    etude::invoke<void>( foo, derived );
    BOOST_ASSERT( derived.member == 8 );
    
    auto x = etude::invoke<double>( bar, derived, 0 );
    STATIC_ASSERT(( std::is_same<double, decltype(x)>::value ));
    BOOST_ASSERT( x == 8 );
  }
}

// test2
// INVOKE(f, t1, t2, ..., tN) is ((*t1).*f)(t2, ..., tN) 
// when f is a pointer to a member function of a class T and
// t1 is not one of the types described in the previous item;
#include <memory>
void test2()
{
  std::unique_ptr<Derived> p( new Derived(3) );
  auto foo = &Base::foo;
  auto bar = &Base::bar;
  
  BOOST_ASSERT( etude::invoke( foo, p ) == 6 );
  BOOST_ASSERT( etude::invoke( bar, p, 2 ) == 10 );
  {
    etude::invoke<void>( foo, p );
    BOOST_ASSERT( p->member == 12 );
    
    auto x = etude::invoke<double>( bar, p, 0 );
    STATIC_ASSERT(( std::is_same<double, decltype(x)>::value ));
    BOOST_ASSERT( x == 12 );
  }
}

// test3
// INVOKE(f, t1, t2, ..., tN) is t1.*f
// when N == 1 and f is a pointer to member data of a class T and
// t1 is an object of type T or a reference to an object of type T
// or a reference to an object of a type derived from T;
void test3()
{
  auto mem = &Base::member;
  
  // object of type T
  BOOST_ASSERT( etude::invoke( mem, Base() ) == 0 );
  {
    auto x = etude::invoke<double>( mem, Base(1) );
    STATIC_ASSERT(( std::is_same<double, decltype(x)>::value ));
    BOOST_ASSERT( x == 1 );
  }
  
  // reference to an object of type T
  Base base(1);
  BOOST_ASSERT( etude::invoke( mem, base ) == 1 );
  {
    etude::invoke( mem, base ) = 2;
    
    auto x = etude::invoke<double>( mem, base );
    STATIC_ASSERT(( std::is_same<double, decltype(x)>::value ));
    BOOST_ASSERT( x == 2 );
  }
  
  // a reference to an object of a type derived from T
  Derived derived(2);
  BOOST_ASSERT( etude::invoke( mem, derived ) == 2 );
  {
    etude::invoke( mem, derived ) *= 2;
    
    auto x = etude::invoke<double>( mem, derived );
    STATIC_ASSERT(( std::is_same<double, decltype(x)>::value ));
    BOOST_ASSERT( x == 4 );
  }
}

// test4
// INVOKE(f, t1, t2, ..., tN) is (*t1).*f
// when N == 1 and f is a pointer to member data of a class T and
// t1 is not one of the types described in the previous item;
void test4()
{
  std::unique_ptr<Derived> p( new Derived(3) );
  auto mem = &Base::member;
  
  BOOST_ASSERT( etude::invoke( mem, p ) == 3 );
  {
    auto x = etude::invoke<double>( mem, p );
    STATIC_ASSERT(( std::is_same<double, decltype(x)>::value ));
    BOOST_ASSERT( x == 3 );
  }
}

// f(t1, t2, ..., tN) in all other cases.
int f(){ return 42; }
void test5()
{
  auto x = etude::invoke( f );
  STATIC_ASSERT(( std::is_same<int, decltype(x)>::value ));
  BOOST_ASSERT( x == 42 );
  
  auto lambda = [&]( int i ){ return x += i; };
  
  auto y = etude::invoke<bool>( lambda, 13 );
  STATIC_ASSERT(( std::is_same<bool, decltype(y)>::value ));
  BOOST_ASSERT( x == 55 && y == true );
}

int main()
{
  test1();
  test2();
  test3();
  test4();
  test5();
}
