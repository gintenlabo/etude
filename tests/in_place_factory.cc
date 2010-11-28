#include "../etude/in_place_factory.hpp"

// 一応実体化テスト
template class etude::in_place_factory<>;
template class etude::in_place_factory<int>;

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

template<class T, class InPlace>
std::unique_ptr<T> make_unique( InPlace && x ) {
  // TODO: T::operator new 対策 && 例外安全
  void* const vp = operator new( sizeof(T) );
  return std::unique_ptr<T>( etude::apply_in_place<T>( std::forward<InPlace>(x), vp ) );
}

#include <iostream>
#include <typeinfo>

int main()
{
  int i = 0;
  
  std::unique_ptr<fuga> x;
  x = make_unique<fuga>( etude::in_place( make_unique<hoge>( boost::in_place( i, 2u ) ) ) );
  
  std::cout << x->p->x << std::endl;
  std::cout << x->p->y << std::endl;
}
