//
//  etude/memory/apply_in_place.hpp に対するテスト
//
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//
#include "../../etude/memory/apply_in_place.hpp"

// #include <boost/test/minimal.hpp>  // うごかない＞＜
#include <boost/assert.hpp>

#include <boost/utility/in_place_factory.hpp>
#include <boost/utility/typed_in_place_factory.hpp>

#include <set>
#include <boost/noncopyable.hpp>

// user-defined in-place factory
struct my_in_place_factory
  : boost::in_place_factory_base
{
  // apply_in_place において完璧な転送が行われているかチェック
  template<class T>
  void apply( void* p ) {
    // if non-const, pass 2 args
    ::new(p) T( 1, 2 );
  }
  template<class T>
  void apply( void* p ) const {
    // if const, pass 1 args
    ::new(p) T( 1 );
  }
  // rvalue-reference に対処したいけど、無理
};

// user-defined typed in-place factory
template<class T>
struct my_typed_in_place_factory
  : boost::typed_in_place_factory_base
{
  typedef T value_type;
  
  // apply_in_place において完璧な転送が行われているかチェック
  void apply( void* p ) {
    // if non-const, pass 2 args
    ::new(p) T( 1, 2 );
  }
  void apply( void* p ) const {
    // if const, pass 1 args
    ::new(p) T( 1 );
  }
  // 同じく rvalue-reference に対処したいけど、無理
};

struct tested
  : private boost::noncopyable
{
  template<class... Args>
  tested( Args&&... )
    : argc_( sizeof...(Args) )
  {
    bool const succeeded = get_object_list().insert(this).second;
    BOOST_ASSERT( succeeded );
  }
  ~tested() {
    std::size_t erased = get_object_list().erase(this);
    BOOST_ASSERT( erased == 1 );
  }
  
  bool is_initialized() const {
    return get_object_list().find(this) != get_object_list().end();
  }
  int passed_arguments() const {
    return argc_;
  }
  
  static bool count_existing_instances() {
    return get_object_list().size();
  }
  
 private:
  typedef std::set<tested const*> object_list_t;
  
  static object_list_t& get_object_list() {
    static object_list_t object_list;
    return object_list;
  }
  
  int const argc_;
  
};

#include <type_traits>
typedef std::aligned_storage<sizeof(tested), alignof(tested)>::type tested_storage;

template<class T>
struct pseudo_destructor_call
{
  void operator()( T* p ) const {
    p->~T();
  }
  void operator()( T& x ) const {
    x.~T();
  }
};

#include <memory>
typedef std::unique_ptr<tested, pseudo_destructor_call<tested>> tested_ptr;

#include <utility>

template<class InPlace,
  class = typename std::enable_if<
    etude::is_in_place_applyable<InPlace, tested>::value
  >::type
>
bool check_if_in_place_applyable( InPlace && x, int argc ) {
  using etude::apply_in_place;
  
  tested_storage buf;
  BOOST_ASSERT( !reinterpret_cast<tested*>(&buf)->is_initialized() );
  
  tested_ptr p( apply_in_place<tested>( std::forward<InPlace>(x), &buf ) );
  BOOST_ASSERT( p->is_initialized() );
  BOOST_ASSERT( p->passed_arguments() == argc );
  
  p.reset();
  BOOST_ASSERT( !reinterpret_cast<tested*>(&buf)->is_initialized() );
  
  return true;
}
// if not in-place applyable
template<class T>
bool check_if_in_place_applyable( T const&, ... ) {
  return false;
}

template<class T>
T const& to_const( T& x ) { return x; }

void apply_in_place_check()
{
  // not a in-place factory
  BOOST_ASSERT( !check_if_in_place_applyable( 1, 0 ) );
  BOOST_ASSERT( !check_if_in_place_applyable( tested(), 0 ) );
  
  // 普通の in-place factory
  // rvalue
  BOOST_ASSERT( check_if_in_place_applyable( boost::in_place(), 0 ) );
  // lvalue
  auto in_place_0 = boost::in_place();
  BOOST_ASSERT( check_if_in_place_applyable( in_place_0, 0 ) );
  // const-lvalue
  BOOST_ASSERT( check_if_in_place_applyable( to_const(in_place_0), 0 ) );
  
  // もいっちょ
  // rvalue
  BOOST_ASSERT( check_if_in_place_applyable( boost::in_place(0), 1 ) );
  // lvalue
  auto in_place_1 = boost::in_place(1);
  BOOST_ASSERT( check_if_in_place_applyable( in_place_1, 1 ) );
  // const-lvalue
  BOOST_ASSERT( check_if_in_place_applyable( to_const(in_place_1), 1 ) );
  
  // other in-place factories
  BOOST_ASSERT( check_if_in_place_applyable( boost::in_place( 2, 3 ), 2 ) );
  BOOST_ASSERT( check_if_in_place_applyable( boost::in_place( "a", "b", 1, 2, 3 ), 5 ) );
  
  // user-defined in-place factory
  // rvalue
  BOOST_ASSERT( check_if_in_place_applyable( my_in_place_factory(), 2 ) );
  // lvalue
  my_in_place_factory my_in_place;
  BOOST_ASSERT( check_if_in_place_applyable( my_in_place, 2 ) );
  // const-lvalue
  BOOST_ASSERT( check_if_in_place_applyable( to_const(my_in_place), 1 ) );


  // typed in-place factory を普通の in-place factory として使う場合のテスト
  
  // typed in-place factory associated with a type other than tested
  BOOST_ASSERT( !check_if_in_place_applyable( boost::in_place<int>(), 0 ) );
  // ユーザ定義のクラス
  BOOST_ASSERT( !check_if_in_place_applyable( my_typed_in_place_factory<int>(), 0 ) );
  // exactly same type required
  BOOST_ASSERT( !check_if_in_place_applyable( boost::in_place<tested const>(1), 1 ) );
  
  // 正しく型指定された typed in-place factory
  // rvalue
  BOOST_ASSERT( check_if_in_place_applyable( boost::in_place<tested>(), 0 ) );
  // lvalue
  auto typed_in_place_0 = boost::in_place<tested>();
  BOOST_ASSERT( check_if_in_place_applyable( typed_in_place_0, 0 ) );
  // const-lvalue
  BOOST_ASSERT( check_if_in_place_applyable( to_const(typed_in_place_0), 0 ) );
  
  // もいっちょ
  // rvalue
  BOOST_ASSERT( check_if_in_place_applyable( boost::in_place<tested>(2), 1 ) );
  // lvalue
  auto typed_in_place_2 = boost::in_place<tested>("hoge", "fuga");
  BOOST_ASSERT( check_if_in_place_applyable( typed_in_place_2, 2 ) );
  // const-lvalue
  BOOST_ASSERT( check_if_in_place_applyable( to_const(typed_in_place_2), 2 ) );
  
  // ユーザ定義
  // rvalue
  BOOST_ASSERT( check_if_in_place_applyable( my_typed_in_place_factory<tested>(), 2 ) );
  // lvalue
  my_typed_in_place_factory<tested> my_typed_in_place;
  BOOST_ASSERT( check_if_in_place_applyable( my_typed_in_place, 2 ) );
  // const-lvalue
  BOOST_ASSERT( check_if_in_place_applyable( to_const(my_typed_in_place), 1 ) );

} // apply_in_place_check()


// apply_typed_in_place に対するチェック

template<class TypedInPlace,
  class = typename std::enable_if<
    etude::is_typed_in_place_factory<TypedInPlace>::value &&
    etude::is_in_place_applyable<TypedInPlace, tested>::value
  >::type
>
bool check_if_typed_in_place_applyable( TypedInPlace && x, int argc ) {
  using etude::apply_typed_in_place;
  
  tested_storage buf;
  BOOST_ASSERT( !reinterpret_cast<tested*>(&buf)->is_initialized() );
  
  tested_ptr p( apply_typed_in_place( std::forward<TypedInPlace>(x), &buf ) );
  BOOST_ASSERT( p->is_initialized() );
  BOOST_ASSERT( p->passed_arguments() == argc );
  
  p.reset();
  BOOST_ASSERT( !reinterpret_cast<tested*>(&buf)->is_initialized() );
  
  return true;
}
// if not in-place applyable
template<class T>
bool check_if_typed_in_place_applyable( T const&, ... ) {
  return false;
}
void apply_typed_in_place_check()
{
  // not a typed in-place factory
  BOOST_ASSERT( !check_if_typed_in_place_applyable( 1, 0 ) );
  BOOST_ASSERT( !check_if_typed_in_place_applyable( tested(), 0 ) );
  BOOST_ASSERT( !check_if_typed_in_place_applyable( boost::in_place(), 0 ) );
  
  // 普通の in-place factory
  // rvalue
  BOOST_ASSERT( check_if_typed_in_place_applyable( boost::in_place<tested>(), 0 ) );
  // lvalue
  auto in_place_0 = boost::in_place<tested>();
  BOOST_ASSERT( check_if_typed_in_place_applyable( in_place_0, 0 ) );
  // const-lvalue
  BOOST_ASSERT( check_if_typed_in_place_applyable( to_const(in_place_0), 0 ) );
  
  // もいっちょ
  // rvalue
  BOOST_ASSERT( check_if_typed_in_place_applyable( boost::in_place<tested>(0), 1 ) );
  // lvalue
  auto in_place_1 = boost::in_place<tested>(1);
  BOOST_ASSERT( check_if_typed_in_place_applyable( in_place_1, 1 ) );
  // const-lvalue
  BOOST_ASSERT( check_if_typed_in_place_applyable( to_const(in_place_1), 1 ) );
  
  // other in-place factories
  BOOST_ASSERT( check_if_typed_in_place_applyable( boost::in_place<tested>( 2, 3 ), 2 ) );
  BOOST_ASSERT( check_if_typed_in_place_applyable( boost::in_place<tested>( "a", "b", 1, 2, 3 ), 5 ) );
  
  // user-defined in-place factory
  // rvalue
  BOOST_ASSERT( check_if_typed_in_place_applyable( my_typed_in_place_factory<tested>(), 2 ) );
  // lvalue
  my_typed_in_place_factory<tested> my_in_place;
  BOOST_ASSERT( check_if_typed_in_place_applyable( my_in_place, 2 ) );
  // const-lvalue
  BOOST_ASSERT( check_if_typed_in_place_applyable( to_const(my_in_place), 1 ) );

}

int main()
{
  apply_in_place_check();
  apply_typed_in_place_check();
}
