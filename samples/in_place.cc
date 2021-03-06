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
  // また、仮に in_place で得られたファクトリを変数（あるいは関数の引数）に束縛した場合、
  // その変数をコピーすることは出来ない（ rvalue-reference なのでコピーできたら困る）。

  // 変数に束縛したい場合やコピーしたい場合は、 bind_in_place か、
  // または in_place_by_val/in_place_by_ref を使う
  
  // 一般的なのは bind_in_place だが、
  // 先に in_place_by_val/in_place_by_ref の例を挙げておく
  auto in_place_1 = etude::in_place_by_val( z, 8 );
  auto in_place_2 = etude::in_place_by_ref( z, 10 );
  // by_val と by_ref の違いは、変数を値キャプチャするか、参照キャプチャするか
  z = 9;
  x = in_place_1; x->print(); // ( 7, 8 )
  x = in_place_2; x->print(); // ( 9, 10 )
  
  // 注意： rvalue-reference の場合には in_place_by_ref を使っても値キャプチャされる
  // xvalues と prvalues をオーバーロード解決で判別することは出来ないため
  // 明示的に rvalue-reference を束縛したい場合には、
  // etude::in_place_factory<int, std::unique_ptr<T>&&>( i, std::move(p) ) のように、
  // 明示的に型を指定して etude::in_place_factory を構築する必要がある
  
  // bind_in_place を使うことで、参照キャプチャと値キャプチャを使い分けられる
  // デフォルトでは値キャプチャ、 std::ref を使うと参照キャプチャ
  x = etude::bind_in_place( 10, 23, std::ref(z) );
  std::cout << z << std::endl;  // 33
  // この束縛ルールは std::make_tuple と同じ。
  
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
  explicit fuga( int x_ )
    : x( x_ ) {}
  
  fuga( int x_, std::unique_ptr<fuga> && next_ )
    : next( std::move(next_) ), x(x_) {}
  
  std::unique_ptr<fuga> next;
  int x;
  
  friend void print( std::unique_ptr<fuga> const& p ) {
    if( !p ) {
      std::cout << "[]\n";
      return;
    }
    std::cout << boost::format("%1% -> ") % p->x;
    print( p->next );
  }
  
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
  class T = typename etude::typed_in_place_associated<TypedInPlace>::type >
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
  auto p1 = make_unique<fuga>( etude::in_place(1) );
  print( p1 );  // 1 -> []
  
  // リンクさせてみる
  // 今度は typed で
  auto p2 = make_unique( etude::in_place<fuga>( 2, std::move(p1) ) );
  print( p2 );  // 2 -> 1 -> []
  // move したので p1 は空
  print( p1 );  // []
  
  // 変数に格納された in_place から move する例
  // 変数に格納するので bind_in_place を使う
  auto in_place_1 = etude::bind_in_place( 3, std::move(p2) );
  print( p2 );  // move されたので []
  // 構築。 変数に束縛されてるので move しないといけない
  p2 = make_unique<fuga>( std::move(in_place_1) );
  print( p2 );  // 3 -> 2 -> 1 -> []
  
  // 変数に格納された typed in-place からの構築
  auto in_place_2 = etude::bind_in_place<fuga>( 0, std::move(p1) );
  // typed in-place でも、 in-place として使える（型を明示して構築できる）。
  p1 = make_unique<fuga>( std::move(in_place_2) );
  print( p1 );  // 0 -> []
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
