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

#include <utility>
#include <type_traits>

// T 型を格納できるストレージ
#include "../../etude/memory/storage.hpp"
using etude::storage_of;

// デストラクタ呼び出しファンクタ
#include "../../etude/memory/pseudo_destructor_call.hpp"
using etude::pseudo_destructor_call;

#include "lifetime_check.hpp"

#endif  // #ifndef ETUDE_TEST_MEMORY_INCLUDED_TEST_UTILITIES_HPP_
