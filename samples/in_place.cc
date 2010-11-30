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
#include <iostream>
#include <boost/format.hpp>

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
  
  void print() {
    std::cout << boost::format("( %1%, %2% )\n") % x % y;
  }
  
};

// 基本的な使い方
// 普通の人は boost.optional への引数としてしか使わないと思うので、それで。

void basic_usage()
{
  boost::optional<hoge> x;
  
  // x = hoge( 1, 2 ); // ダメ
  
  // 代わりにこう使う
  x = etude::in_place( 1, 2 );
  x->print(); // ( 1, 2 )
  // typed in-place factory でもおｋ
  x = etude::in_place<hoge>( 2, 3 );
  
  // boost との違いは、参照を扱えること。
  int z = 0;
  x = etude::in_place( 3, 4, z );
  // x = boost::in_place( 3, 4, z );  // ダメ。
  std::cout << z << std::endl;  // 7
  
  // 注意！ 変数に格納して使い回す場合は in_place は使っちゃダメ
  // auto in_place_0 = etude::in_place( 5, 6 ); // 一時変数も参照キャプチャするので危険
  // 代わりに in_place_by_val/in_place_by_ref を使う
  auto in_place_1 = etude::in_place_by_val( z, 8 );
  auto in_place_2 = etude::in_place_by_ref( z, 10 );
  // 違いは簡単で、変数を値としてキャプチャするか、参照としてキャプチャするか
  z = 9;
  x = in_place_1; x->print(); // ( 7, 8 )
  x = in_place_2; x->print(); // ( 9, 10 )
  
  // in_place_by_val で参照キャプチャしたい場合は、 std::ref を使う
  x = etude::in_place_by_val( 10, 23, std::ref(z) );
  std::cout << z << std::endl;  // 33
  // in_place_by_val の束縛法は std::make_tuple と同じ。
  
  // なお、束縛された引数は、 get_tuple によって tuple として取得できる
  std::cout << std::get<0>( get_tuple(in_place_1) ) << std::endl; // 7
  // ただし、参照を束縛していない限り、代入は出来ない
  // std::get<0>( get_tuple(in_place_1) ) = 42 // ダメ
}


// 発展例。 std::unique_ptr 等を扱う場合
#include <memory>

// サンプルクラス。こんな単純な物でもオリジナルの boost::in_place では無理。
struct fuga
{
  fuga( std::unique_ptr<hoge> && p_ )
    : p( std::move(p_) ) {}
  
  std::unique_ptr<hoge> p;
  
};

// boost::optional だと無理なので、自前で構築関数も用意しないといけない
// 例として、 std::unique_ptr に格納されたオブジェクトを直接作る関数を自作する

// TODO: ユーザ定義された operator delete への対応
// 自前でのメモリ管理って本気で不便…。

// operator delete を呼ぶ関数（例外安全用）
struct deallocator
{
  void operator()( void* p ) const {
    ::operator delete( p );
  }
};

// T を明示して構築
template<class T, class InPlace,
  class = typename std::enable_if<etude::is_in_place_applyable<InPlace, T>::value>::type>
std::unique_ptr<T> make_unique( InPlace && x ) {
  // メモリ確保。結果は apply_in_place で例外が投げられた時の為に一旦 unique_ptr に格納。
  std::unique_ptr<void, deallocator> p( ::operator new( sizeof(T) ) );
  
  using namespace etude;  // なんとなくADLで呼ぶ
  apply_in_place<T>( std::forward<InPlace>(x), p.get() );
  
  return std::unique_ptr<T>( static_cast<T*>( p.release() ) );
}

// TypedInPlace から T を推論する版
// 第二テンプレート引数は型計算と enable_if を兼ねているので、明示的に指定しないこと。
template<class TypedInPlace,
  class T = typename etude::get_in_place_binding_type<TypedInPlace>::type >
inline std::unique_ptr<T> make_unique( TypedInPlace && x ) {
  // 要は InPlace 版と同じ
  std::unique_ptr<void, deallocator> p( ::operator new( sizeof(T) ) );
  
  using namespace etude;
  apply_typed_in_place( std::forward<TypedInPlace>(x), p.get() );
  
  return std::unique_ptr<T>( static_cast<T*>( p.release() ) );
}

// 具体的に使ってみる
void advanced_usage()
{
  // まずポインタを作る
  auto p1 = make_unique<hoge>( etude::in_place(1, 2) );
  
  // move する
  auto p2 = make_unique<fuga>( etude::in_place( std::move(p1) ) );
  // move したので p1 は空
  BOOST_ASSERT( !p1 );
  // 戻しとくか。
  p1 = std::move( p2->p );
  
  // 変数束縛された in_place から move する
  // 注意： rvalue-reference の場合には by_ref でも値キャプチャされる
  // 一時変数かどうかを判別することが出来ないので。
  // 一時変数がないと分かってるなら in_place を使えば参照でキャプチャされる
  auto in_place_1 = etude::in_place_by_ref<fuga>( std::move(p1) );  // typed in-place
  BOOST_ASSERT( !p1 );
  p2 = make_unique( std::move(in_place_1) );  // 中身を move させる旨を書く必要あり
  
  p1 = make_unique( etude::in_place<hoge>(2, 3) );
  auto in_place_2 = etude::in_place_by_val<fuga>( std::move(p1) );  // なんとなく by val
  BOOST_ASSERT( !p1 );
  // typed in-place でも、 in-place として使える例
  p2 = make_unique<fuga>( std::move(in_place_2) );
}


int main()
{
  basic_usage();
  advanced_usage();
  
  // 最後に、 Boost.InPlace から正しく apply_in_place 出来るかチェック
  make_unique<hoge>( boost::in_place( 2, 3 ) );
  make_unique( boost::in_place<hoge>( 4, 5 ) );
  // これが出来ると嬉しい（そして出来る）
  make_unique<hoge>( boost::in_place<hoge>( 4, 5 ) );
  // これはコンパイルエラー。
  // make_unique<int>( boost::in_place<hoge>( 4, 5 ) );
}
