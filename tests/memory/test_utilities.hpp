//
//  memory ライブラリテスト用の小物をまとめたヘッダ
//    今のところ in_place factory 用。
//    後々 etude のライブラリとして独立させる可能性をはらんだものです。
//
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#ifndef ETUDE_TEST_MEMORY_INCLUDED_TEST_UTILITIES_HPP_
#define ETUDE_TEST_MEMORY_INCLUDED_TEST_UTILITIES_HPP_

#include <type_traits>
#include <boost/assert.hpp>
#include <set>

// T 型を格納できるストレージ
#include "../../etude/memory/storage.hpp"
using etude::storage_of;

// デストラクタ呼び出しファンクタ
#include "../../etude/memory/pseudo_destructor_call.hpp"
using etude::pseudo_destructor_call;

// 寿命チェック用オブジェクト
template<class Tag>
struct lifetime_check
{
  lifetime_check() {
    register_instance(this);
  }
  lifetime_check( lifetime_check const& ) {
    register_instance(this);
  }
  ~lifetime_check() {
    unregister_instance(this);
  }
  
  static bool is_initialized( void const* p ) {
    return get_object_list().find(p) != get_object_list().end();
  }
  
  static bool count_existing_instances() {
    return get_object_list().size();
  }
  
 private:
  typedef std::set<void const*> object_list_t;
  
  static object_list_t& get_object_list() {
    static object_list_t object_list;
    return object_list;
  }
  
  static void register_instance( void const* p ){
    bool const succeeded = get_object_list().insert(p).second;
    BOOST_ASSERT( succeeded );
  }
  static void unregister_instance( void const* p ){
    std::size_t erased = get_object_list().erase(p);
    BOOST_ASSERT( erased == 1 );
  }
  
};


#endif  // #ifndef ETUDE_TEST_MEMORY_INCLUDED_TEST_UTILITIES_HPP_
