
In-Place Factories
===================

使い道
  コンストラクタの引数を転送し、メモリ領域を直接初期化する

必要ヘッダ
  ::
    
    // 全ての機能を使う場合。以下の全てのヘッダがインクルードされる
    #include <etude/in_place.hpp>  
    
    // etude::in_place のみ使いたい場合
    #include <etude/memory/in_place_factory.hpp>        // 型指定なし版
    #include <etude/memory/typed_in_place_factory.hpp>  // 型指定あり版
    
    // in-place factory の適用ヘルパ関数
    #include <etude/memory/apply_in_place.hpp>
    
    // 問い合わせ用の小物メタ関数
    #include <etude/memory/is_in_place_factory.hpp>
    #include <etude/memory/is_typed_in_place_factory.hpp>


概要
-----

`Boost.InPlaceFactories <http://www.boost.org/doc/libs/1_45_0/libs/utility/in_place_factories.html>`_ の 
C++0x 版です。rvalue references と variadic templates に対応しています。

InPlaceFactories は、それ単独では価値を見出しにくいライブラリですが、
例えば `Boost.Optional <http://www.boost.org/doc/libs/1_45_0/libs/optional/doc/html/>`_ 
といったライブラリと組み合わせることで、非常に便利に使うことが出来ます。

InPlaceFactories は、あるメモリ領域に、 copy や move に依らず、
直接的にオブジェクトを構築することを可能にします： ::

  #include <boost/scoped_ptr.hpp>
  #include <boost/optional.hpp>
  #include <boost/utility/in_place_factory.hpp>

  // このクラスは boost::scoped_ptr を持っているのでコピー出来ない
  struct hoge
  {
    hoge( int a, std::string const& s );
    ~hoge();
    
    void foo();
    
   private:
    class Impl_;
    boost::scoped_ptr<Impl_> pImpl_
  };
  
  void some_function() {
    boost::optional<hoge> h;
    // でも Boost.Optional に格納できる！
    h = boost::in_place( 1, "hoge" );
    // ...
  }

使い方は、オブジェクトの引数を ``in_place`` という関数で束ねた上で、
InPlaceFactories をサポートしているクラスに渡すだけです。

そのようにして InPlaceFactories を使った場合の分かりやすい恩恵は、上記のように、
コピー出来ないオブジェクトを扱う際の制限が減る、というものですが、
それだけに留まりません。

InPlaceFactories を使うことにより、「構築して copy/move する」と
二段階で初期化していたオブジェクトを、
たった一回、直接的に初期化するのみで構築することが出来るようになるのです。

実際には C++ には RVO という機能があるので、 InPlaceFactories を使わずとも、
コンパイラによって二段階ではなく一段階の初期化に最適化されうるのですが、
その最適化が働くかどうかはコンパイラ依存です。

InPlaceFactories を使えば、原理的には、
コンパイラに依らず、 copy/move を介さない一度の呼び出しで、
あらゆるオブジェクトを初期化出来るようになります。

実際には InPlaceFactories に対応したライブラリはごく一部なので 
そう簡単には行きませんが、少なくとも「何となく便利そうだ」ということは
分かってもらえたんじゃないでしょうか。

で、そんな便利な Boost.InPlaceFactories ですが、現状ではいくらか問題があります。

- デフォルトで引数パックを値キャプチャするので、効率がいささか悪い
- Variadic Templates を使っていないので、型が分かりにくい
- 実装側の問題だが、 apply する時に move してくれない
- 同じく実装側の問題だが、 InPlaceFactory と TypedInPlaceFactory の二種類に対応しなければならない

これらの問題に対処したのが、 Etude.InPlaceFactories です。

Etude.InPlaceFactories は、基本的には Boost.InPlaceFactories と全く同じように扱うことが出来ます。

これは、今まで Boost.InPlaceFactories を使っていた場面を、そのまま Etude.InPlaceFactories 
によって置き換えることができる、ということです： ::

  // 再掲。 hoge の定義は上を参照
  void some_function() {
    boost::optional<hoge> h;
    // Boost.Optional を置き換えずに etude::in_place を使える
    h = etude::in_place( 1, "hoge" );
    // ...
  }

それに加えて Etude.InPlaceFactories では、 rvalue references を扱うことが出来ます： ::

  // etude なら OK, boost だと NG
  XXX xxx = etude::in_place( std::unique_ptr<foo>( new foo() ) );

その為には実装側で rvalue references に対応する必要がありますが、
その実装を助けるための各種のヘルパ関数、ヘルパメタ関数も 
Etude.InPlaceFactories には用意されています。

また、地味ですが、 Etude.InPlaceFactories では、 ``in_place`` 関数の結果を保持するクラスの名前が、
``etude::in_place_factory<Args...>`` および ``etude::typed_in_place_factory<T, Args...>`` と
明確に定義されている為、いざという時に便利です。

詳しい使い方、および Boost との差異については、以下で述べます。


使い方（利用側）
----------------

under construction...


使い方（実装側）
----------------

under construction...


詳細データ
-----------

under construction...
