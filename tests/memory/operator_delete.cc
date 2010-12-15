//
//  etude/memory/operator_delete.hpp に対するテスト
//
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#include "../../etude/memory/operator_delete.hpp"
#include "../../etude/memory/default_deallocate.hpp"

#include <boost/assert.hpp>
#include <memory>

template<class T>
void test()
{
  {
    T* p = new T();
    p->~T();
    etude::operator_delete<T>(p);
  }
  
  int const n = 10;
  
  {
    T* p = new T[n];
    for( int i = n; i --> 0; ) {
      (p+i)->~T();
    }
    etude::operator_array_delete<T>(p);
  }
  
  // unique_ptr を使った例
  {
    std::unique_ptr<T, etude::default_deallocate<T>> p( new T() );
    p->~T();
  }
  {
    std::unique_ptr<T[], etude::default_deallocate<T[]>> p( new T[n] );
    for( int i = n; i --> 0; ) {
      p[i].~T();
    }
  }
}

template<class T>
void bad_test()
{
  T* p = new T();
  p->~T();
  ::operator delete(p); // あっ
  
  int const n = 10;
  
  p = new T[n];
  for( int i = n; i --> 0; ) {
    (p+i)->~T();
  }
  ::operator delete[](p); // …。
}

struct hoge
{
  static void* operator new( std::size_t size ) {
    void* const p = ::operator new( size );
    instances_() += 1;
    return p;
  }
  static void operator delete( void* p ) {
    ::operator delete(p);
    instances_() -= 1;
  }
  
  static void* operator new []( std::size_t size ) {
    void* const p = ::operator new[]( size );
    arrays_() += 1;
    return p;
  }
  static void operator delete[]( void* p ) {
    ::operator delete[](p);
    arrays_() -= 1;
  }
  
  static int count_instances() {
    return instances_();
  }
  static int count_arrays() {
    return arrays_();
  }
  
  int x;
  
 private:
  static int& instances_() {
    static int instances = 0;
    return instances;
  }
  static int& arrays_() {
    static int arrays = 0;
    return arrays;
  }
  
};

int main()
{
  // 普通の型に対して
  test<int>();
  
  // operator delete がある型について
  test<hoge>();
  
  BOOST_ASSERT( hoge::count_instances() == 0 );
  BOOST_ASSERT( hoge::count_arrays() == 0 );
  
  // 一応、 operator_delete じゃなければダメな理由を。
  bad_test<hoge>();
  // ::operator delete の場合、確かに T::operator delete は呼ばれない。
  BOOST_ASSERT( hoge::count_instances() == 1 );
  BOOST_ASSERT( hoge::count_arrays() == 1 );
  
}
