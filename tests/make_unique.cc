//
//  make_unique のテストです。
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../etude/make_unique.hpp"

#include <type_traits>
#include <utility>
#include <boost/assert.hpp>

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

struct hoge
{
  std::unique_ptr<int> p;
  explicit hoge( std::unique_ptr<int>&& p_ )
    : p( std::move(p_) ) {}
};

int main()
{
  auto p1 = etude::make_unique<int>();
  auto p2 = etude::make_unique<hoge>( std::move(p1) );
  
  STATIC_ASSERT(( std::is_same<decltype(p1), std::unique_ptr<int>>::value ));
  STATIC_ASSERT(( std::is_same<decltype(p2), std::unique_ptr<hoge>>::value ));
  
  BOOST_ASSERT( !p1 && p2 );
}
