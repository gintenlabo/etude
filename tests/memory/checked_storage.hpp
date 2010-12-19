//
//  checked_storage: 解放漏れやらなんやらをチェックするストレージクラス
//    テスト用ユーティリティの一つです。
//
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_TEST_MEMORY_INCLUDED_CHECKED_STORAGE_HPP_
#define ETUDE_TEST_MEMORY_INCLUDED_CHECKED_STORAGE_HPP_

#include "lifetime_check.hpp"
#include "../../etude/memory/storage.hpp"
#include <memory>

template<class T, class Tag = void>
class checked_storage
  : private lifetime_check< checked_storage<T, Tag> >,
    private etude::storage<T>
{
  typedef checked_storage self_type;
  typedef etude::storage<T> storage_type;
  typedef lifetime_check<checked_storage<T, Tag>> lifetime_checker;
  
  template<class U>
  friend class lifetime_check;
  
  static void* allocate_() {
    self_type* const p = new self_type();
    return static_cast<storage_type*>(p);
  }
  static void deallocate_( void* vp ) {
    storage_type* const p = static_cast<storage_type*>(vp);
    delete static_cast<self_type*>(p);
  }
  
 public:
  using storage_type::address;
  
  // 指定されたポインタは確保されているか？
  static bool is_allocated( void const* p ) {
    return lifetime_checker::is_initialized(p);
  }
  // 確保されたオブジェクトの数を返す
  static int count_allocated_instance() {
    return lifetime_checker::count_existing_instance();
  }
  
  struct deallocator
  {
    typedef void result_type;
    
    void operator()( void* p ) const {
      deallocate_(p);
    }
  
  };
  
  typedef std::unique_ptr<void, deallocator> storage_ptr;
  
  static storage_ptr allocate() {
    return storage_ptr( allocate_() );
  }

};

// 自由関数版
template<class T, class Tag = void,
  class Result = typename checked_storage<T>::storage_ptr
>
inline Result checked_allocate() {
  return checked_storage<T, Tag>::allocate();
}

#endif  // #ifndef ETUDE_TEST_MEMORY_INCLUDED_CHECKED_STORAGE_HPP_
