#include "../etude/in_place.hpp"

// 一応実体化テスト
template class etude::in_place_factory<>;
template class etude::in_place_factory<int>;
template class etude::typed_in_place_factory<int>;
template class etude::typed_in_place_factory<int, int>;

// boost::optional でテスト
#include <boost/assert.hpp>
#include <boost/optional.hpp>
#include <boost/noncopyable.hpp>
#include <functional>   // for std::ref

struct hoge
  : private boost::noncopyable
{
  hoge( int x_, int y_ )
    : x(x_), y(y_) {}
  
  // 参照をとるコンストラクタ
  // 人工的だけど勘弁して下さい＞＜
  hoge( int x_, int y_, int& z )
    : x(x_), y(y_)
  {
    z = x + y;
  }
  
  int x, y;
};

// 基本的な使い方
// 普通の人は boost.optional への引数としてしか使わないと思うので、それで。
void basic_usage()
{
  boost::optional<hoge> x;
  
  // x = hoge( 1, 2 ); // ダメ
  
  // 代わりにこう使う
  x = etude::in_place( 1, 2 );
  // typed in-place factory でもおｋ
  x = etude::in_place<hoge>( 2, 3 );
  
  // boost との違いは、参照を扱えること。
  int z = 0;
  x = etude::in_place( 3, 4, z );
  // x = boost::in_place( 3, 4, z );  // ダメ。
  BOOST_ASSERT( z == 7 );
  
  // 注意！ 変数に格納して使い回す場合は in_place は使っちゃダメ
  // auto in_place_0 = etude::in_place( 5, 6 ); // 一時変数も参照キャプチャするので危険
  // 代わりに in_place_by_val/in_place_by_ref を使う
  auto in_place_1 = etude::in_place_by_val( z, 8 );
  auto in_place_2 = etude::in_place_by_ref( z, 10 );
  // 違いは簡単で、変数を値としてキャプチャするか、参照としてキャプチャするか
  z = 9;
  x = in_place_1; BOOST_ASSERT( x->x == 7 );
  x = in_place_2; BOOST_ASSERT( x->x == 9 );  // 参照キャプチャ！
  
  // in_place_by_val の場合に参照キャプチャしたい場合は、 std::ref を使う
  x = etude::in_place_by_val( 10, 23, std::ref(z) );
  BOOST_ASSERT( z == 33 );
  // in_place_by_val の束縛法は std::make_tuple と同じ。
  
  // なお、束縛された引数は、 get_tuple によって tuple として取得できる
  BOOST_ASSERT( std::get<0>( get_tuple(in_place_1) ) == 7 );
  // ただし、参照を束縛していない限り、代入は出来ない
  // std::get<0>( get_tuple(in_place_1) ) = 42 // ダメ
}


// 発展例。 std::unique_ptr を使う（これは boost では無理）
#include <memory>
struct fuga
{
  fuga( std::unique_ptr<hoge> && p_ )
    : p( std::move(p_) ) {}
  
  std::unique_ptr<hoge> p;
  
};

template<class T, class InPlace,
  class = typename std::enable_if<etude::is_in_place_applyable<InPlace, T>::value>::type>
std::unique_ptr<T> make_unique( InPlace && x ) {
  using namespace etude;
  // TODO: T::operator new 対策 && 例外安全
  void* const vp = operator new( sizeof(T) );
  return std::unique_ptr<T>( apply_in_place<T>( std::forward<InPlace>(x), vp ) );
}
template<class TypedInPlace,
  class T = typename etude::get_in_place_binding_type<TypedInPlace>::type >
inline std::unique_ptr<T> make_unique( TypedInPlace && x ) {
  using namespace etude;
  // 同じく T::operator new 対策 && 例外安全はサボり中
  void* const vp = operator new( sizeof(T) );
  return std::unique_ptr<T>( apply_typed_in_place( std::forward<TypedInPlace>(x), vp ) );
}

#include <iostream>
#include <typeinfo>

int main()
{
  basic_usage();
  
  int i = 0;
  
  auto const args = etude::in_place_by_ref( i, 2u );
  i = 1;
  
  auto proxy = etude::in_place_by_val<fuga>( make_unique<hoge>( args ) );
  
  std::unique_ptr<fuga> x = make_unique( std::move(proxy) );
  
  std::cout << x->p->x << std::endl;
  std::cout << x->p->y << std::endl;
  
  // Boost.InPlace から正しく apply_in_place 出来るかチェック
  make_unique<hoge>( boost::in_place( 2, 3 ) );
  make_unique( boost::in_place<hoge>( 4, 5 ) );
  // これが出来ると嬉しい
  make_unique<hoge>( boost::in_place<hoge>( 4, 5 ) );
  make_unique<hoge>( etude::in_place<hoge>( 4, 5 ) );
  // これはコンパイルエラー。
  // make_unique<int>( boost::in_place<hoge>( 4, 5 ) );
  
  std::cout << std::boolalpha;
  std::cout << etude::is_in_place_applyable<etude::in_place_factory<>, hoge>::value << std::endl;
  std::cout << etude::is_in_place_applyable<etude::typed_in_place_factory<hoge>, hoge>::value << std::endl;
  std::cout << etude::is_in_place_applyable<etude::typed_in_place_factory<int>, hoge>::value << std::endl;
}
