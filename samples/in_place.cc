#include "../etude/in_place.hpp"

// 一応実体化テスト
template class etude::in_place_factory<>;
template class etude::in_place_factory<int>;
template class etude::typed_in_place_factory<int>;
template class etude::typed_in_place_factory<int, int>;

// boost::optional でテスト
#include <boost/optional.hpp>
#include <boost/noncopyable.hpp>

struct hoge
  : private boost::noncopyable
{
  hoge( int x_, int y_ )
    : x(x_), y(y_) {}
  
  int x, y;
};

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
