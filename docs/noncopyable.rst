
Noncopyable
============

使い道
  コピーできないクラスであることを明示する

必要ヘッダ
  ::
    
    #include <etude/noncopyable.hpp>


概要
-----

`boost::noncopyable <http://www.boost.org/doc/libs/1_45_0/libs/utility/utility.htm#Class_noncopyable>`_ の C++0x 版です。 ``= delete;`` 指定を使って実装されています。

boost::noncopyable との違いは、

- trivially dafault-constructible である
- trivially destructible である

の二点。

どちらも普通に C++ を書く分には気を使わなくていいのですが、
これにより思わぬ最適化が働く場合もあるので、そうなっていて損はないです。


使い方
-------

単純に private 継承するだけです。 ::

  #include <etude/noncopyable.hpp>
  
  class hoge
    : private etude::noncopyable
  {
    // 実装
    // ...
  };
  
  // ...
  
  int main() {
    hoge x;
    hoge y = x; // コンパイルエラー！
  }


public 継承でも「コピーを禁止する」意図からすれば問題はありませんが、
多重継承時に共通の基底クラスになる可能性や、
またリスコフの置換原則を考えても、 private 継承にしたほうが無難です。


詳細データ
-----------

<etude/noncopyable.hpp> ::

  namespace etude {
   namespace noncopyable_ { // ADL 回避
   
    struct noncopyable
    {
      noncopyable() = default;
      
      noncopyable( noncopyable const& )    = delete;
      void operator=( noncopyable const& ) = delete;
      noncopyable( noncopyable&& )         = delete;
      void operator=( noncopyable&& )      = delete;
      
    };
   
   }
   using namespace noncopyable_;
  }

etude::noncopyable は、以下の条件を満たす単純な空のクラスです：

- non-copy-constructible
- non-move-constructible
- non-copy-assignable
- non-move-assignable
- trivially default-constructible
- trivially destructible

通常このクラスは、 private 継承することによりコピー禁止を明示する目的で使われます。

etude::noncopyable を継承したことが原因で、そのクラスの associated namespaces に
名前空間 etude が追加されることはありません。
