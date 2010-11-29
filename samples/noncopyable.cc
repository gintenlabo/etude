#include "../etude/noncopyable.hpp"
#include <boost/noncopyable.hpp>

struct hoge
  : private etude::noncopyable
{
};
struct fuga
  : private boost::noncopyable
{
};

struct derived
  : hoge, private boost::noncopyable
{
};

#include <type_traits>
#include <iostream>
int main()
{
  hoge h;
  (void)h;
  
  // hoge x = h;
  // h = h;
  // hoge y = hoge();
  // h = hoge();
  
  std::cout << std::boolalpha;
  std::cout << std::has_trivial_default_constructor<hoge>::value << std::endl;
  std::cout << std::has_trivial_destructor<hoge>::value << std::endl;
  
  std::cout << std::has_trivial_default_constructor<fuga>::value << std::endl;
  std::cout << std::has_trivial_destructor<fuga>::value << std::endl;
  
  std::cout << std::is_empty<derived>::value << std::endl;
}
