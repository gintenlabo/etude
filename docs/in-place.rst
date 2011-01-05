.. index::
  single: In-Place Factories

==================
In-Place Factories
==================

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
====

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
================

基本的な使用法
--------------

``boost::in_place(...)`` と書く場所で、 ``etude::in_place(...)`` と書く。これだけです。

Etude.InPlaceFactory は、 Boost.InPlaceFactory の完全な上位互換となっているので、
Boost.InPlaceFactory を使えるライブラリであるならば、そのライブラリがきちんと実装されている限り、
Etude.InPlaceFactory も問題なく扱うことが出来ます。

Boost との差異は、 Perfect Forward を使っていることです。
これよにり、参照を取るコンストラクタであっても、問題なく転送することが出来ます。

::
  
  struct Hoge {
    int& r;
    
    explicit Hoge( int& r_ )
      : r( r_ ) {}
    
  };
  
  int i;
  
  // NG
  // boost::optional<Hoge> x( boost::in_place(i) );

  // OK
  boost::optional<Hoge> x( etude::in_place(i) );
  

また、これは受け取る側が Etude.InPlaceFactory に対応している場合に限りますが、
Move Semantics により、一時オブジェクトをコピーすることなく渡すことが出来ます。

::

  // InPlaceFactory を受け入れられる何か
  etude_in_place_acceptable x = etude::in_place( std::unique_ptr<int>( new int() ) );

これにより、 ``boost::in_place`` よりも効率よく引数を転送できます。

注意点
~~~~~~

「 Etude.InPlaceFactory は Boost.InPlaceFactory の完全な上位互換となっている」

と書きましたが、これは厳密には正しくありません。

Etude.InPlaceFactory が Boost.InPlaceFactory の全ての機能を含んでいるのは事実ですが、
``etude::in_place``\ :ref:`¶<in_place>`\ :ref:`¶<in_place typed>` という関数自身については、
``boost::in_place`` とは若干異なった動作をする関数になっています。

具体的には、 ``etude::in_place`` によって作られたオブジェクトを、

- コピーする場合
- そのスコープで変数に束縛する場合
- 関数の戻り値として使う場合

には、コンパイルエラーになったり、さらに悪いことには、
コンパイルエラーにすらならず異常動作を引き起こす可能性があります。

``etude::in_place`` の結果を渡した先でコンパイルエラーになる場合や、
``etude::in_place`` を呼び出した結果を

- そのスコープで変数に束縛する場合
- 関数の戻り値として使う場合

には、 ``etude::in_place`` ではなく
``etude::bind_in_place``\ :ref:`¶<bind_in_place>`\ :ref:`¶<bind_in_place typed>`
を使うようにしてください ::

  int i = 5;

  boost::optional<T> x( etude::in_place( i, std::string("a") ) ); // 問題なし
  
  // ダメ。
  // std::string の一時オブジェクトは auto in_place = ～; の実行後に破棄される
  // auto in_place = etude::in_place( i, std::string("b") );
  // x = in_place; // dangling reference !
  
  // 代わりに bind_in_place を使う
  auto in_place = etude::bind_in_place( i, std::string("b") );
  x = in_place; // 問題なし
  
そうすることで、これらの問題は、完全に回避することができます。

``etude::bind_in_place`` に渡した引数は、参照ではなく値として束縛され、
``std::ref`` や ``std::cref`` 経由で渡した引数のみが参照で束縛されます。

詳細な動作は :ref:`こちら<in_place>` を参照してください。

.. note::
  ``etude::in_place`` が、このような
  「コンパイルエラーにすらならず、異常動作を引き起こしうる」
  という危険性をあえて残しているのは、
  ``etude::in_place`` の設計方針として、 Perfect Forward であることと、
  一切の余計なコピーを行わないことを重視しているためです。
  
  その設計に伴う危険性ですが、
  
  - コンパイルエラーにならずに異常動作を引き起こすの可能性があるのは、あくまで
    
    - ``etude::in_place`` を呼び出した **そのスコープで** 、 ``auto`` 等を使って変数に束縛した場合
    - ``etude::in_place`` の呼び出し結果を、 ``decltype`` 等で、関数の戻り値に **直接** 使った場合
    
    のみであり、それ以外の場合には、コンパイルエラーによって通知されること。
  
  - 基本的に、 ``in_place`` で作られたオブジェクトを、すぐに使わず変数に束縛したり、
    あるいは関数から返すようなことは、意図して行わない限りは起こらず、
    仮に起こったとしても、よほど混み入ったコードでもない限り、コードを見ればすぐに分かること。
  
  - それに加え、従来の Boost.InPlaceFactories を使っていたコードは、基本的に
    C++98/03 であり、 C++98/03 の範疇では、 ``auto`` を使って変数に束縛する、ということや、
    ``decltype`` を使って戻り値に利用する、ということは行えなかったことから、
    従来の ``boost::in_place`` を機械的に ``etude::in_place``
    に書き換えても、問題は起こりにくいと考えられること。
  
  - それとは別に、このような問題は、標準ライブラリの ``std::forward_as_tuple``
    でも生じるものであり、 ``etude::in_place`` に固有の問題というわけではないこと。
  
  上記の理由により十分に回避可能である、と判断しています。


より高度な使用法
----------------

タプルに束ねられた引数リストから InPlaceFactory への変換
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Etude.InPlaceFactory は、 ``std::tuple`` をはじめとした、各種タプルからの構築をサポートしています。

何らかの理由により、タプルによって予め束ねられた引数リストから InPlaceFactory
を構築したくなった場合、通常ならば、プリプロセッサや補助クラス（例：
``etude::indices``\ :ref:`¶<indices>` ）を使ったトリッキーなコードを書く必要がありますが、
Etude.InPlaceFactory を使えば、
``in_place_from_tuple``\ :ref:`¶<in_place_from_tuple>`\ :ref:`¶<in_place_from_tuple typed>`
を使うことで、ごく自然に tuple を unpack することが可能になっています。

その際、渡すタプルが ``std::tuple`` である必要はありません。
``std::tuple_size`` と ``std::tuple_element`` 、 ``std::get``
さえ適切に定義されていれば、問題なく unpack することが可能です。

普段、この機能を使うことは滅多に無いでしょうが、覚えておいて損はない筈です。


InPlaceFactory の要件を満たすクラスの自作
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

何らかの理由で、 InPlaceFactory の仕組み自体は使いたいが、
コンストラクタ呼び出しはライブラリ経由ではなく自前で行ないたい、という場合を考えます。

例えば、 private constructor 経由で構築したい場合などが、その最たるものでしょう。

::
  
  struct Hoge
  {
    // interfaces
    void foo();
    void bar();
    // ...
    
    // 構築関数
    static std::shared_ptr<Hoge> create();
    
   private:
    Hoge() {} // スタック上に構築されると困る場合とか
    
  };

上記のようなクラスで、 ``create()`` 関数内において InPlaceFactory を受け入れるライブラリ（例えば
Boost.Optional ）を使いたい時、普通に ``etude::in_place`` を使うのでは、 ``Hoge::Hoge()`` は
``private`` なので、 ``etude::in_place_factory`` から呼び出すことが出来ずエラーになります。

その場合、 InPlaceFactory の要件を満たした自前のクラスを作れば、アクセス制限を回避しつつ、
InPlaceFactory の恩恵を最大限に満たすことが出来るようになります。
InPlaceFactory の要件は、以下の通りです：

- クラス ``InPlace`` が以下の条件を充たす場合、
  ``InPlace`` は InPlaceFactory の要件を満たします：

  - ``InPlace`` が ``boost::in_place_factory_base`` を ``public`` 継承している
  - ``InPlace const x = ...;  void* addr;`` に対し
    ``x.template apply<T>(addr);`` という形のメンバ関数テンプレート呼び出しが可能である

- クラス ``TypedInPlace`` が以下の条件を充たす場合、
  ``TypedInPlace`` は TypedInPlaceFactory の要件を満たします：

  - ``TypedInPlace`` が ``boost::typed_in_place_factory_base`` を ``public`` 継承している
  - ``typename TypedInPlace::value_type`` が何かしらの型に ``typedef`` されている
  - ``TypedInPlace const x = ...;  void* addr;`` に対し
    ``x.apply(addr);`` という形のメンバ関数呼び出しが可能である

例として、上記の ``Hoge`` の ``create()`` 周りを実装してみます： ::

  struct Hoge
  {
    // interfaces
    void foo();
    void bar();
    // ...
    
    static std::shared_ptr<Hoge> create();
    
   private:
    Hoge() {}
    
    // この場合は TypedInPlace が妥当
    struct Construct
      : boost::typed_in_place_factory_base
    {
      typedef Hoge value_type;
      
      void apply( void* addr ) const {
        ::new(addr) Hoge();
      }
    };
    
  };
  
  // 構築関数 本体
  inline std::shared_ptr<Hoge> Hoge::create() {
    auto p = std::make_shared<boost::optional<Hoge>>();
    
    // *p = etude::in_place();  // ダメ
    *p = Hoge::Construct();
    return std::shared_ptr<Hoge>( std::move(p), p->get_ptr() );
  }

このように ``Hoge::create()`` を実装することで、単純に ::

  inline std::shared_ptr<Hoge> Hoge::create() {
    return std::shared_ptr<Hoge>( new Hoge() );
  }

と書くのに比べ、メモリ確保回数を減らすことが可能になります。


使い方（実装側）
================

under construction...


詳細データ
==========

.. index::
  single: In-Place Factories; is_in_place_factory

.. _is_in_place_factory:

``is_in_place_factory``
-----------------------

必要ヘッダ
  ::
    
    #include <etude/memory/is_in_place_factory.hpp>

定義
  ::
  
    #include <boost/utility/in_place_factory.hpp>
  
    namespace etude {
    
      using boost::in_place_factory_base;
    
      template<class T>
      struct is_in_place_factory
        : etude::integral_constant<bool, see-below> {};
      
    }

``etude::is_in_place_factory<T>`` は、 ``T`` が（CV修飾された） InPlaceFactory
（ ``boost::in_place_factory_base`` から派生したクラス）または InPlaceFactory への参照の場合には
``std::true_type`` を、そうでない場合には ``std::false_type`` を継承するメタ関数です。
  
.. hint::
  
  特別な理由がなければ、このメタ関数を直接使うのではなく、
  より一般的に使える ``etude::is_in_place_applyable<InPlace, T>``\ :ref:`¶<is_in_place_applyable>`
  を使う方がよいでしょう。


.. index::
  single: In-Place Factories; in_place_factory

.. _in_place_factory:

``in_place_factory``
--------------------

必要ヘッダ
  ::
    
    #include <etude/memory/in_place_factory.hpp>

定義
  ::

    #include "is_in_place_factory.hpp"
    #include <tuple>
  
    namespace etude {
      
      // class template in_place_factory
      template<class... Args>
      struct in_place_factory
        : boost::in_place_factory_base
      {
        typedef std::tuple<Args...> tuple_type;
        
        // construct, copy
        template<class... Types>
        explicit in_place_factory( Types&& ...args );
        
        in_place_factory( tuple_type const& t );
        in_place_factory( tuple_type && t );
        
        template<class...Types>
        in_place_factory( in_place_factory<Types...> const& src );
        template<class...Types>
        in_place_factory( in_place_factory<Types...> && src );
        
        // gcc 4.5.0 does not support implicit move
        in_place_factory( in_place_factory const& ) = default;
        in_place_factory( in_place_factory && ) = default;
        
        // no assign operator defined
        in_place_factory& operator=( in_place_factory const& ) = delete;
        
        // application
        template<class T>
        T* apply( void* addr ) const;
        template<class T>
        T* move_apply( void* addr );
        
        // get arguments as tuple
        tuple_type const& get_tuple() const;
        tuple_type && move_tuple();
        
      };
      
      // in_place_factory non-member functions
      template<class... Args>
      inline std::tuple<Args> const& get_tuple( in_place_factory<Args...> const& x );
      template<class... Args>
      inline std::tuple<Args> && get_tuple( in_place_factory<Args...> && x );
      
      template<class T, class... Args>
      inline T* apply_in_place( in_place_factory<Args...> & x, void* addr );
      template<class T, class... Args>
      inline T* apply_in_place( in_place_factory<Args...> const& x, void* addr );
      template<class T, class... Args>
      inline T* apply_in_place( in_place_factory<Args...> && x, void* addr );
      
      
      // function template in_place
      template<class... Args>
      inline in_place_factory<Args&&...> in_place( Args&& ...args );
      template<class... Args>
      inline in_place_factory<see-below...> bind_in_place( Args&& ...args );
      template<class... Args>
      inline in_place_factory<Args...> in_place_by_ref( Args&& ...args );
      template<class... Args>
      inline in_place_factory<see-below...> in_place_by_val( Args&& ...args );
      template<class Tuple>
      inline in_place_factory<see-below...> in_place_from_tuple( Tuple && t );
      
    }

コンストラクタを呼び出す為の引数を pack したクラスです。

このクラスは Boost.InPlaceFactory の上位互換として使えます。


.. _in_place_factory types:

``in_place_factory`` types
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. compound::

  :: 

    typedef std::tuple<Args...> tuple_type;

  ``in_place_factory<Args...>`` が内部に保持する引数パックの型です。


.. _in_place_factory ctors:

``in_place_factory`` constructors
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. compound::

  ::
  
    template<class... Types> explicit in_place_factory( Types&&... args );
  
  与えられた引数を保持する ``in_place_factory<Args...>`` を構築します。
    
  ``Types...`` から ``Args...`` が変換可能である（
  ``etude::is_convertible<etude::types<Types...>, etude::types<Args...>>::value``\
  :ref:`¶<is_convertible>` が ``true`` である）必要があります。

.. compound::

  ::
  
    in_place_factory( tuple_type const& x );
    in_place_factory( tuple_type && x );

  渡されたタプルに格納された引数リストから ``in_place_factory<Args...>`` を構築します。

  .. note::
    
    このコンストラクタは都合により、任意のタプルから構築できるようには実装されていません。
    
    言語仕様上、ユーザ定義の型変換は一度しか行われないため、
    このコンストラクタに渡すタプルの型が ``std::tuple<Args...>`` と厳密に一致しない場合、
    たとえタプルの中身が変換可能であっても、コンパイルエラーとなります。
    
    具体的な例を挙げると、 ::
    
      etude::in_place_factory<double> x = std::make_tuple(1);
    
    は、 ``std::make_tuple(1)`` の結果である ``std::tuple<int>`` と、
    ``etude::in_place_factory<double>`` のコンストラクタが要求する ``std::tuple<double>``
    の型が厳密に一致しないため、 ill-formed です。
    
    型の厳密に一致しないタプルから ``in_place_factory<Args...>`` を構築したい場合には、
    ``in_place_from_tuple`` を用いて型変換を行うようにしてください。

.. compound::

  :: 
  
    template<class... Types> in_place_factory( in_place_factory<Types...> const& src );
    template<class... Types> in_place_factory( in_place_factory<Types...> && src );
    
  異なる型の引数を保持する ``in_place_factory`` からの型変換を提供します。
  
  ``Types...`` から ``Args...`` が変換可能である（
  ``etude::is_convertible<etude::types<Types...>, etude::types<Args...>>::value``\
  :ref:`¶<is_convertible>` が ``true`` である）必要があります。
  
  .. hint::
  
    主に ::
    
      etude::in_place_factory<double> x = etude::in_place( 0 );
    
    のように使います。


``in_place_factory`` applications
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. warning::
  
  これらの関数はアライメントや例外安全などに問題のある、本質的に安全でない関数であり、
  安易な呼び出しは未定義動作を容易に引き起こします。
  
  上記の説明の意味が分からない場合は、決してこの関数を自分で呼び出してはいけません。


.. _in_place_factory apply:

.. compound::

  ::
  
    template<class T> T* apply( void* addr ) const;
  
  コンストラクタで渡された引数を用いて、指定されたメモリ領域に ``T`` 型のオブジェクトを構築し、
  構築されたオブジェクトへのポインタを返します。
  
  この関数は、以下のコードと同等です： ::
  
    ::new (addr) T( args... )
    
  ただし ``args...`` はコンストラクタで渡された引数です。
  

.. _in_place_factory move_apply:

.. compound::

  ::
  
    template<class T> T* move_apply( void* addr );
  
  コンストラクタで渡された引数を用いて、指定されたメモリ領域に ``T`` 型のオブジェクトを構築し、
  構築されたオブジェクトへのポインタを返します。
  
  その際、この関数は、格納している引数を move します。
  
  この関数は、以下のコードと同等です： ::
  
    ::new (addr) T( std::forward<Args>(args)... )
    
  ただし ``args...`` はコンストラクタで渡された引数です。
  
  .. note::
  
    この関数が呼び出されると、内部に格納された変数は move され、有効な状態ではなくなります。
    
    これはつまり、この関数の呼び出し後は、このクラスに対するあらゆる操作は
    undefined behavior を引き起こす可能性がある、ということです。


``in_place_factory`` observers
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. _in_place_factory get_tuple:

.. compound::

  ::
  
    tuple_type const& get_tuple() const;
  
  コンストラクタで渡された引数を格納したタプルへの const 参照を得ます。
  

.. _in_place_factory move_tuple:

.. compound::

  ::
  
    tuple_type && move_tuple();
  
  コンストラクタで渡された引数を格納したタプルを move します。
  
  .. note::
  
    この関数が呼び出されると、内部に格納された変数は move され、有効な状態ではなくなります。
    
    これはつまり、この関数の呼び出し後は、このクラスに対するあらゆる操作は
    undefined behavior を引き起こす可能性がある、ということです。


``in_place_factory`` non-member functions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. compound::

  ::
  
    template<class... Args>
    inline std::tuple<Args...> const& get_tuple( in_place_factory<Args...> const& x );
  
  ``x.get_tuple()``\ :ref:`¶<in_place_factory get_tuple>` と同じです。

.. compound::

  ::
  
    template<class... Args>
    inline std::tuple<Args...> && get_tuple( in_place_factory<Args...> && x );
  
  ``x.move_tuple()``\ :ref:`¶<in_place_factory move_tuple>` と同じです。

.. index::
  single: In-Place Factories; apply_in_place

.. _in_place_factory apply_in_place:

.. compound::

  ::
  
    template<class T, class... Args>
    inline T* apply_in_place( in_place_factory<Args...> & x, void* addr );
    
    template<class T, class... Args>
    inline T* apply_in_place( in_place_factory<Args...> const& x, void* addr );
  
  ``x.template apply<T>( addr )``\ :ref:`¶<in_place_factory apply>` と同じです。
  
.. compound::

  ::
  
    template<class T, class... Args>
    inline T* apply_in_place( in_place_factory<Args...> && x, void* addr );
  
  ``x.template move_apply<T>( addr )``\ :ref:`¶<in_place_factory move_apply>` と同じです。

.. hint::
  
  これらの関数は、対象オブジェクトが lvalue か rvalue かによって呼び出す関数を切り替える、
  といった面倒な手間をなくすために提供されています。


.. index::
  single: In-Place Factories; in_place

.. _in_place:

function template ``in_place``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. note::

  これらの関数群は、現状あまり良い名前とは言えないため、
  より相応しい名前が見つかった場合には変更するかもしれません。


.. compound::

  ::
  
    template<class... Args>
    inline in_place_factory<Args&&...> in_place( Args&&... args );
  
  与えられた引数への右辺値参照を束縛した ``in_place_factory<Args...>`` を構築します。
  
  この関数は与えられた引数を「そのままに」束縛します。
  
  これはつまり、 ::
  
    etude::apply_in_place<T>( etude::in_place( args... ), addr )
  
  と、 ::
  
    ::new(addr) T( args... )
  
  が、コンパイラの最適化に依らず、意味論的に同じ動作をする、ということです。
  
  この動作は、通常、速度と意味論の双方において望ましいものですが、
  右辺値参照を扱っているため、幾つかの場合において問題が発生します。
  
  まず、この関数呼び出しの結果をローカル変数に格納することはできません： ::
  
    auto x = etude::in_place( hoge() ); // ダメ。
    // x の参照する一時オブジェクトはここで破棄される
  
  次に、この関数呼び出しの結果を関数の引数として使う場合、
  その引数をコピーすることは出来ません。 ::
  
    template<class InPlace>
    void f( InPlace && x ) {
      auto y = x; // x をコピー
      // ...
    }
    
    f( etude::in_place(1, 2) ); // コピー出来ないのでコンパイルエラー
  
  これらの問題を避けてより安全に使いたい場合は、 ``bind_in_place``\ :ref:`¶<bind_in_place>`
  を使ってください。
  
  .. note::
  
    引数を与えず単に ``etude::in_place()`` と使う場合には、
    変数への束縛もコピーも、安全に行うことが出来ます。
    
    その場合に わざわざ ``etude::bind_in_place()`` と書く必要はありません。


.. index::
  single: In-Place Factories; bind_in_place

.. _bind_in_place:

.. compound::

  ::
  
    template<class... Args>
    inline in_place_factory<see-below...> bind_in_place( Args&&... args );
  
  与えられた引数を値として束縛した ``in_place_factory<Args...>`` を構築します。
  
  この関数の戻り値は
  ``etude::in_place_factory< typename etude::decay_and_strip<Args>::type... >``\
  :ref:`¶<decay_and_strip>` で与えられます。
  
  これは ``std::make_tuple`` で行われる型変換と同一です。
  つまり与えられた引数は基本的に値として（配列や関数の場合はポインタとして）束縛され、
  参照として束縛させたい場合には ``std::ref`` や ``std::cref`` を使う、ということです。
  
  この関数は本質的に安全ですが、一方で原則的に全ての引数を値によりキャプチャするため、
  コピーコストの高いオブジェクトの場合は、明示的に
  ``std::ref`` や ``std::cref`` を使うなどして、効率化を図る必要があるでしょう。


.. index::
  single: In-Place Factories; in_place_by_ref

.. _in_place_by_ref:

.. compound::

  ::
  
    template<class... Args>
    inline in_place_factory<Args...> in_place_by_ref( Args&&... args );
  
  与えられた引数を束縛した ``in_place_factory<Args...>`` を構築します。
  
  この関数は ``bind_in_place``\ :ref:`¶<bind_in_place>` とは違い、
  一時オブジェクト以外は全て参照として束縛するので、
  最低限の安全性を保ったまま、余計なコピーコストを省略することが出来ます。
  
  一方で、ローカル変数を参照として束縛するため、
  関数の戻り値として この関数の結果を使うことは出来ませんし、
  参照先のオブジェクトが変更された場合には意図しない結果になることもあります： ::
  
    int i = 0;
    auto x = etude::in_place_by_ref(i);
    i = 42; // x の「中身」は 42 になる
  
  一般に、この挙動は、不可解なバグを引き起こす原因となり得るので、
  基本的に、この関数ではなく ``bind_in_place``\ :ref:`¶<bind_in_place>` を使い、
  コピーコストが気になる場合には、 ``std::ref`` や ``std::cref`` を使って
  「この変数は参照で渡している」
  ということを明示的に示した方がよりよい場合が多いかと思います。
  
  また、この関数の限界として、 move されたオブジェクトは参照で渡すことが出来ません： ::
  
    std::unique_ptr<int> p( new int() );
    auto x = etude::in_place_by_ref( std::move(p) );  // この時点で p は move される
  
  これは、関数のオーバーロード解決時に、純粋な一時変数と
  move されたオブジェクトとを区別することは不可能である為です。
  
  もし move されたオブジェクトを ``in_place_factory`` に格納したければ、 ::
  
    std::unique_ptr<int> p( new int() );
    auto x = etude::in_place_factory<std::unique_ptr<int>&&>( std::move(p) );
  
  のように、明示的に ``in_place_factory`` のコンストラクタを呼び出してください。

.. index::
  single: In-Place Factories; in_place_by_val

.. _in_place_by_val:

.. compound::

  ::
  
    template<class... Args>
    inline in_place_factory<see-below...> in_place_by_val( Args&&... args );
  
  与えられた引数を値として束縛した ``in_place_factory<Args...>`` を構築します。
  
  この関数の戻り値は
  ``etude::in_place_factory< typename std::decay<Args>::type... >``
  で与えられます。
  
  ``bind_in_place``\ :ref:`¶<bind_in_place>` との違いは、
  ``std::reference_wrapper<T>`` に包まれたオブジェクトであっても そのまま束縛する、という点です。
  
  ``std::reference_wrapper<T>`` 型の引数を取るコンストラクタに対して\
  安全に引数を転送したい場合には、こちらを使うのが良いでしょう。

.. index::
  single: In-Place Factories; in_place_from_tuple

.. _in_place_from_tuple:

.. compound::

  ::

    template<class Tuple>
    inline in_place_factory<see-below...> in_place_from_tuple( Tuple && t );
  
  引数をパックしたタプルから ``in_place_factory<Args...>`` を構築します。
  
  この関数の戻り値は、
  ``etude::in_place_factory< typename etude::tuple_element< Indices, typename std::decay<Tuple>::type >::type... >``
  で与えられます。 ただし ``Indices`` は ``0`` から ``etude::tuple_size<Tuple>::value - 1``
  までの各インデックスを順に並べたものになります。
  
  例えば、 ::
  
    std::tuple<int, double, char const*> t;
    auto x = etude::in_place_from_tuple( t );
  
  の ``x`` は、 ``etude::in_place_factory< int, double, char const* >`` になります。
  
  .. hint::
  
    この関数は、コンストラクタ呼び出しに関し、擬似的なタプルの unpack として機能します。
    
    つまり、 ::
    
      etude::apply_in_place<T>( etude::in_place_from_tuple(t), addr );
    
    は、あたかも ::
    
      ::new(addr) T( unpack(t) );
    
    のように動作します。


.. index::
  single: In-Place Factories; is_typed_in_place_factory

.. _is_typed_in_place_factory:

``is_typed_in_place_factory``
-----------------------------

必要ヘッダ
  ::
    
    #include <etude/memory/is_typed_in_place_factory.hpp>

定義
  ::
  
    #include <boost/utility/typed_in_place_factory.hpp>
  
    namespace etude {
    
      using boost::typed_in_place_factory_base;
    
      template<class T>
      struct is_typed_in_place_factory
        : etude::integral_constant<bool, see-below> {};
      
    }

``etude::is_typed_in_place_factory<T>`` は、 ``T`` が（CV修飾された） TypedInPlaceFactory
（ ``boost::typed_in_place_factory_base`` から派生したクラス）または
TypedInPlaceFactory への参照の場合には ``std::true_type`` を、そうでない場合には
``std::false_type`` を継承するメタ関数です。

.. hint::
  
  特別な理由がなければ、このメタ関数を直接使うのではなく、
  ``etude::typed_in_place_associated<TypedInPlace>``\ :ref:`¶<typed_in_place_associated>`
  を使って、関連付けられた型も同時に取得した方が楽です。


.. index::
  single: In-Place Factories; typed_in_place_associated

.. _typed_in_place_associated:

``typed_in_place_associated``
-----------------------------

必要ヘッダ
  ::
    
    #include <etude/memory/is_typed_in_place_factory.hpp>

定義
  ::
  
    namespace etude {
    
      template<class T>
      struct typed_in_place_associated;
      
    }

``T`` が（CV修飾された） TypedInPlaceFactory または TypedInPlaceFactory への参照の場合、
``typename etude::typed_in_place_associated<T>::type`` は
``typename std::remove_reference<T>::type::value_type`` に定義されます。

そうでなければ ``typed_in_place_associated<T>::type`` は定義されません。

.. hint::
  
  このメタ関数は、例えば関数テンプレートのデフォルトテンプレート引数を用いて ::
  
    template<class TypedInPlace,
      class T = typename etude::typed_in_place_associated<TypedInPlace>::type>
    T* some_function( TypedInPlace && x );
  
  のように使うとよいでしょう。


.. index::
  single: In-Place Factories; typed_in_place_factory

.. _typed_in_place_factory:

``typed_in_place_factory``
--------------------------

必要ヘッダ
  ::
    
    #include <etude/memory/typed_in_place_factory.hpp>

定義
  ::

    #include "is_typed_in_place_factory.hpp"
    #include "in_place_factory.hpp"
    #include <tuple>
  
    namespace etude {
      
      // class template typed_in_place_factory
      template<class T, class... Args>
      struct typed_in_place_factory
        : boost::typed_in_place_factory_base
      {
        typedef T value_type;
        typedef std::tuple<Args...> tuple_type;
        
        // construct, copy
        template<class... Types>
        explicit typed_in_place_factory( Types&& ...args );
        
        typed_in_place_factory( tuple_type const& t );
        typed_in_place_factory( tuple_type && t );
        
        template<class... Types>
        typed_in_place_factory( typed_in_place_factory<T, Types...> const& src );
        template<class... Types>
        typed_in_place_factory( typed_in_place_factory<T, Types...> && src );
        
        template<class... Types>
        typed_in_place_factory( in_place_factory<Types...> const& t );
        template<class... Types>
        typed_in_place_factory( in_place_factory<Types...> && t );
        
        // gcc 4.5.0 does not support implicit move
        typed_in_place_factory( typed_in_place_factory const& ) = default;
        typed_in_place_factory( typed_in_place_factory && ) = default;
        
        // no assign operator defined
        typed_in_place_factory& operator=( typed_in_place_factory const& ) = delete;
        
        
        // application
        T* apply( void* addr ) const;
        T* move_apply( void* addr );
        
        
        // get arguments as tuple
        tuple_type const& get_tuple() const;
        tuple_type && move_tuple();
        
      };
      
      // in_place_factory non-member functions
      template<class T, class... Args>
      inline std::tuple<Args> const& get_tuple( typed_in_place_factory<T, Args...> const& x );
      template<class T, class... Args>
      inline std::tuple<Args>&& get_tuple( typed_in_place_factory<T, Args...> && x );
      
      template<class T, class... Args>
      inline T* apply_typed_in_place( typed_in_place_factory<T, Args...> & x, void* addr );
      template<class T, class... Args>
      inline T* apply_typed_in_place( typed_in_place_factory<T, Args...> const& x, void* addr );
      template<class T, class... Args>
      inline T* apply_typed_in_place( typed_in_place_factory<T, Args...> && x, void* addr );
      
      
      // function template in_place
      template<class T, class... Args>
      inline typed_in_place_factory<T, Args&&...> in_place( Args&& ...args );
      template<class T, class... Args>
      inline typed_in_place_factory<T, see-below...> bind_in_place( Args&& ...args );
      template<class T, class... Args>
      inline typed_in_place_factory<T, Args...> in_place_by_ref( Args&& ...args );
      template<class T, class... Args>
      inline typed_in_place_factory<T, see-below...> in_place_by_val( Args&& ...args );
      template<class T, class Tuple>
      inline typed_in_place_factory<T, see-below...> in_place_from_tuple( Tuple && t );
      
    }

特定の型に関連付けられた引数の pack です。

このクラスは Boost.TypedInPlaceFactory の上位互換として使えます。


.. _typed_in_place_factory types:

``typed_in_place_factory`` types
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. compound::

  :: 

    typedef T value_type;

  ``typed_in_place_factory<T, Args...>`` に関連付けられた型です。


.. compound::

  :: 

    typedef std::tuple<Args...> tuple_type;

  内部に保持する引数パックの型です。
    
  ``typed_in_place_factory<T, Args...>`` は、内部にこの型のメンバを一つだけ保持します。


.. _typed_in_place_factory ctors:

``typed_in_place_factory`` constructors
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. compound::

  ::
  
    template<class... Types> explicit typed_in_place_factory( Types&& ...args );
  
  与えられた引数を保持する ``typed_in_place_factory<T, Args...>`` を構築します。
    
  ``Types...`` から ``Args...`` が変換可能である（
  ``etude::is_convertible<etude::types<Types...>, etude::types<Args...>>::value``\
  :ref:`¶<is_convertible>` が ``true`` である）必要があります。

.. compound::

  ::
  
    typed_in_place_factory( tuple_type const& x );
    typed_in_place_factory( tuple_type && x );

  渡されたタプルに格納された引数リストから ``typed_in_place_factory<T, Args...>`` を構築します。

  .. note::
    
    このコンストラクタは都合により、任意のタプルから構築できるようには実装されていません。
    
    言語仕様上、ユーザ定義の型変換は一度しか行われないため、
    このコンストラクタに渡すタプルの型が ``std::tuple<Args...>`` と厳密に一致しない場合、
    たとえタプルの中身が変換可能であっても、コンパイルエラーとなります。
    
    詳しくは ``in_place_factory`` の :ref:`該当記事<in_place_factory ctors>` を参考にしてください。

.. compound::

  :: 
  
    template<class... Types>
    typed_in_place_factory( typed_in_place_factory<T, Types...> const& src );
    template<class... Types>
    typed_in_place_factory( typed_in_place_factory<T, Types...> && src );
  
  異なる型の引数 pack を保持する ``typed_in_place_factory`` からの型変換を提供します。
  
  ``Types...`` から ``Args...`` が変換可能である（
  ``etude::is_convertible<etude::types<Types...>, etude::types<Args...>>::value``\
  :ref:`¶<is_convertible>` が ``true`` である）必要があります。
  

.. compound::

  :: 
  
    template<class... Types> typed_in_place_factory( in_place_factory<Types...> const& src );
    template<class... Types> typed_in_place_factory( in_place_factory<Types...> && src );
    
  特定の型に関連付けられていない ``in_place_factory``\ :ref:`¶<in_place_factory>`
  からの型変換を提供します。
  
  ``Types...`` から ``Args...`` が変換可能である（
  ``etude::is_convertible<etude::types<Types...>, etude::types<Args...>>::value``\
  :ref:`¶<is_convertible>` が ``true`` である）必要があります。
  
  .. note::
  
    主に ::
    
      etude::typed_in_place_factory<hoge, double> = etude::in_place( 0 );
    
    のように使うためのものです。


``typed_in_place_factory`` applications
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. warning::
  
  これらの関数はアライメントや例外安全などに問題のある、本質的に安全でない関数であり、
  安易な呼び出しは未定義動作を容易に引き起こします。
  
  上記の説明の意味が分からない場合は、決してこの関数を自分で呼び出してはいけません。


.. _typed_in_place_factory apply:

.. compound::

  ::
  
    T* apply( void* addr ) const;
  
  コンストラクタで渡された引数を用いて、指定されたメモリ領域に ``T`` 型のオブジェクトを構築し、
  構築されたオブジェクトへのポインタを返します。
  
  この関数は、以下のコードと同等です： ::
  
    ::new (addr) T( args... )
    
  ただし ``args...`` はコンストラクタで渡された引数です。
  

.. _typed_in_place_factory move_apply:

.. compound::

  ::
  
    T* move_apply( void* addr );
  
  コンストラクタで渡された引数を用いて、指定されたメモリ領域に ``T`` 型のオブジェクトを構築し、
  構築されたオブジェクトへのポインタを返します。
  
  その際、この関数は、格納している引数を move します。
  
  この関数は、以下のコードと同等です： ::
  
    ::new (addr) T( std::forward<Args>(args)... )
    
  ただし ``args...`` はコンストラクタで渡された引数です。
  
  .. note::
  
    この関数が呼び出されると、内部に格納された変数は move され、有効な状態ではなくなります。
    
    これはつまり、この関数の呼び出し後は、このクラスに対するあらゆる操作は
    undefined behavior を引き起こす可能性がある、ということです。


``in_place_factory`` observers
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. _typed_in_place_factory get_tuple:

.. compound::

  ::
  
    tuple_type const& get_tuple() const;
  
  コンストラクタで渡された引数を格納したタプルへの const 参照を得ます。
  

.. _typed_in_place_factory move_tuple:

.. compound::

  ::
  
    tuple_type const& move_tuple() const;
  
  コンストラクタで渡された引数を格納したタプルを move します。
  
  .. note::
  
    この関数が呼び出されると、内部に格納された変数は move され、有効な状態ではなくなります。
    
    これはつまり、この関数の呼び出し後は、このクラスに対するあらゆる操作は
    undefined behavior を引き起こす可能性がある、ということです。


``typed_in_place_factory`` non-member functions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. compound::

  ::
  
    template<class T, class... Args>
    inline std::tuple<Args...> const& get_tuple( typed_in_place_factory<T, Args...> const& x );
  
  ``x.get_tuple()``\ :ref:`¶<typed_in_place_factory get_tuple>` と同じです。

.. compound::

  ::
  
    template<class T, class... Args>
    inline std::tuple<Args...> && get_tuple( typed_in_place_factory<T, Args...> && x );
  
  ``x.move_tuple()``\ :ref:`¶<typed_in_place_factory move_tuple>` と同じです。

.. index::
  single: In-Place Factories; apply_typed_in_place
  
.. _typed_in_place_factory apply_typed_in_place:

.. compound::

  ::
  
    template<class T, class... Args>
    inline T* apply_typed_in_place( typed_in_place_factory<T, Args...> & x, void* addr );
    
    template<class T, class... Args>
    inline T* apply_typed_in_place( typed_in_place_factory<T, Args...> const& x, void* addr );
  
  ``x.apply( addr )``\ :ref:`¶<typed_in_place_factory apply>` と同じです。
  
.. compound::

  ::
  
    template<class T, class... Args>
    inline T* apply_typed_in_place( typed_in_place_factory<T, Args...> && x, void* addr );
  
  ``x.move_apply( addr )``\ :ref:`¶<typed_in_place_factory move_apply>` と同じです。

.. hint::
  
  これらの関数は、対象オブジェクトが lvalue か rvalue かによって呼び出す関数を切り替える、
  といった面倒な手間をなくすために提供されています。
  
  
.. index::
  single: In-Place Factories; in_place

.. _in_place typed:

function template ``in_place`` (typed version)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. note::

  これらの関数群は、現状あまり良い名前とは言えないため、
  より相応しい名前が見つかった場合には変更するかもしれません。


.. compound::

  ::
  
    template<class T, class... Args>
    inline typed_in_place_factory<T, Args&&...> in_place( Args&&... args );
  
  与えられた引数への右辺値参照を束縛した ``typed_in_place_factory<T, Args...>`` を構築します。
  
  ``in_place_factory<Args...>``\ :ref:`¶<in_place_factory>` 版の
  ``in_place``\ :ref:`¶<in_place>` との使い分けですが、
  関数呼び出し時に、構築対象の型を明示して ::
  
    etude::in_place<SomeObject>( a, b )
  
  のように使った場合には、こちらが呼ばれます。
  
  この関数は、戻り値が TypedInPlaceFactory である以外は、
  ``in_place_factory<Args...>``\ :ref:`¶<in_place_factory>` 版の
  ``in_place``\ :ref:`¶<in_place>` と同じように動作します。
  詳しい解説は :ref:`そちら<in_place>` を参照してください。

.. index::
  single: In-Place Factories; bind_in_place

.. _bind_in_place typed:

.. compound::

  ::
  
    template<class T, class... Args>
    inline typed_in_place_factory<T, see-below...> bind_in_place( Args&&... args );
  
  与えられた引数を値として束縛した ``typed_in_place_factory<T, Args...>`` を構築します。
  
  この関数の戻り値は
  ``etude::typed_in_place_factory< T, typename etude::decay_and_strip<Args>::type... >``\
  :ref:`¶<decay_and_strip>` で与えられます。
  
  ``in_place_factory<Args...>``\ :ref:`¶<in_place_factory>` 版の
  ``bind_in_place``\ :ref:`¶<bind_in_place>` との使い分けに関しては
  :ref:`こちら<in_place typed>` を、
  渡された引数がどのように束縛されるかは
  :ref:`こちら<bind_in_place>` を、それぞれ参照してください。

.. index::
  single: In-Place Factories; in_place_by_ref

.. _in_place_by_ref typed:

.. compound::

  ::
  
    template<class T, class... Args>
    inline typed_in_place_factory<T, Args...> in_place_by_ref( Args&&... args );
  
  与えられた引数を束縛した ``typed_in_place_factory<T, Args...>`` を構築します。
  
  ``in_place_factory<Args...>``\ :ref:`¶<in_place_factory>` 版の
  ``in_place_by_ref``\ :ref:`¶<in_place_by_ref>` との使い分けに関しては
  :ref:`こちら<in_place typed>` を、
  渡された引数がどのように束縛されるかは
  :ref:`こちら<in_place_by_ref>` を、それぞれ参照してください。

.. index::
  single: In-Place Factories; in_place_by_val

.. _in_place_by_val typed:

.. compound::

  ::
  
    template<class T, class... Args>
    inline typed_in_place_factory<T, see-below...> in_place_by_val( Args&&... args );
  
  与えられた引数を値として束縛した ``typed_in_place_factory<T, Args...>`` を構築します。
  
  この関数の戻り値は
  ``etude::typed_in_place_factory< T, typename std::decay<Args>::type... >``
  で与えられます。
  
  ``in_place_factory<Args...>``\ :ref:`¶<in_place_factory>` 版の
  ``in_place_by_val``\ :ref:`¶<in_place_by_val>` との使い分けに関しては
  :ref:`こちら<in_place typed>` を、
  渡された引数がどのように束縛されるかは
  :ref:`こちら<in_place_by_val>` を、それぞれ参照してください。

.. index::
  single: In-Place Factories; in_place_from_tuple

.. _in_place_from_tuple typed:

.. compound::

  ::

    template<class T, class Tuple>
    inline typed_in_place_factory<T, see-below...> in_place_from_tuple( Tuple && t );
  
  引数をパックしたタプルから ``typed_in_place_factory<T, Args...>`` を構築します。
  
  ``in_place_factory<Args...>``\ :ref:`¶<in_place_factory>` 版の
  ``in_place_from_tuple``\ :ref:`¶<in_place_from_tuple>` との使い分けに関しては
  :ref:`こちら<in_place typed>` を、
  渡された引数がどのように束縛されるかは
  :ref:`こちら<in_place_from_tuple>` を、それぞれ参照してください。


.. index::
  single: In-Place Factories; is_in_place_applyable

.. _is_in_place_applyable:

``is_in_place_applyable``
-------------------------

必要ヘッダ
  ::
    
    #include <etude/memory/apply_in_place.hpp>

定義
  ::
  
    #include "is_in_place_factory.hpp"
    #include "is_typed_in_place_factory.hpp"

    namespace etude {
    
      template<class InPlace, class T>
      struct is_in_place_applyable
        : std::integral_constant<bool, see-below> {};
    
    }

.. compound::

  このメタ関数は、
  ``is_in_place_factory<InPlace>::value``\ :ref:`¶<is_in_place_factory>`
  が ``true`` の場合、または
  ``typename typed_in_place_associated<InPlace>::type``\ :ref:`¶<typed_in_place_associated>`
  が存在し ``T`` 型である場合には ``std::true_type`` を継承し、
  それ以外の場合には ``std::false_type`` を継承します。
  
  .. hint::
    このメタ関数は、主に後述の ``apply_in_place``\ :ref:`¶<apply_in_place>`
    を呼び出すか否かを判定する為に使います。


.. index::
  single: In-Place Factories; apply_in_place

.. _apply_in_place:

``apply_in_place``
------------------

必要ヘッダ
  ::
    
    #include <etude/memory/apply_in_place.hpp>


定義
  ::
  
    #include "is_in_place_factory.hpp"
    #include "is_typed_in_place_factory.hpp"

    namespace etude {
    
      template<class T, class InPlace>
      inline T* apply_in_place( InPlace && x, void* addr );
      
      /*
      // in header "in_place_factory.hpp"
      template<class T, class... Args>
      inline T* apply_in_place( in_place_factory<Args...> & x, void* addr );
      template<class T, class... Args>
      inline T* apply_in_place( in_place_factory<Args...> const& x, void* addr );
      template<class T, class... Args>
      inline T* apply_in_place( in_place_factory<Args...> && x, void* addr );
      */
    
    }

.. compound::

  ``addr`` で示される領域に、 ``InPlace`` を適用することで ``T`` 型のオブジェクトを構築します。

  この関数は、
  ``typename typed_in_place_associated<InPlace>::type``\ :ref:`¶<typed_in_place_associated>`
  が存在し ``T`` 型である場合には ::
  
    apply_typed_in_place( std::forward<InPlace>(x), addr );
  
  を\ :ref:`¶<apply_typed_in_place>` 、それ以外の場合には ::
  
    std::forward<InPlace>(x).template apply<T>( addr );
  
  を呼び出します。
  
  この関数の戻り値は構築されたオブジェクトのアドレスになります。

  .. note::

    ``etude::in_place_factory<Args...>``\ :ref:`¶<in_place_factory>` および
    ``etude::typed_in_place_factory< T, Args... >``\ :ref:`¶<typed_in_place_factory>`
    に対する ``apply_in_place`` 呼び出しは、
    overload resolution により、各々のヘッダで定義された
    ``apply_in_place``\ :ref:`¶<in_place_factory apply_in_place>` または
    ``apply_typed_in_place``\ :ref:`¶<typed_in_place_factory apply_typed_in_place>`
    が選択されます。

  .. hint::

    この関数は、 Boost.InPlaceFactory と Boost.TypedInPlaceFactory 、
    ``etude::in_place_factory<Args...>``\ :ref:`¶<typed_in_place_factory>` 、
    および ``etude::typed_in_place_factory< T, Args... >``\ :ref:`¶<typed_in_place_factory>`
    を、統一的なインターフェイスで扱うために用意されています。


.. index::
  single: In-Place Factories; apply_typed_in_place

.. _apply_typed_in_place:

``apply_typed_in_place``
------------------------

必要ヘッダ
  ::
    
    #include <etude/memory/apply_in_place.hpp>


定義
  ::
  
    #include "is_typed_in_place_factory.hpp"

    namespace etude {
    
      template<class TypedInPlace,
        class T = typename std::remove_reference<TypedInPlace>::type::value_type
      >
      inline T* apply_typed_in_place( TypedInPlace && x, void* addr );
    
      /*
      // in header "typed_in_place_factory.hpp"
      template<class T, class... Args>
      inline T* apply_typed_in_place( typed_in_place_factory<T, Args...> & x, void* addr );
      template<class T, class... Args>
      inline T* apply_typed_in_place( typed_in_place_factory<T, Args...> const& x, void* addr );
      template<class T, class... Args>
      inline T* apply_typed_in_place( typed_in_place_factory<T, Args...> && x, void* addr );
      */
      
    }

.. compound::

  ``addr`` で示される領域に、 ``TypedInPlace`` を適用して ``T`` 型のオブジェクトを構築します。
  ``T`` 型は ``TypedInPlace`` に関連付けられた型で、自動的に型推論されます。

  この関数は ::
  
    std::forward<TypedInPlace>(x).apply( addr );
  
  を呼び出します。
  
  戻り値は構築されたオブジェクトのアドレスになります。

  .. note::

    ``etude::typed_in_place_factory< T, Args... >``\ :ref:`¶<typed_in_place_factory>`
    に対する ``apply_typed_in_place`` 呼び出しは、 overload resolution により、
    ``<etude/memory/typed_inplace_factory.hpp>``\ :ref:`¶<typed_in_place_factory>`
    で定義された
    ``apply_typed_in_place``\ :ref:`¶<typed_in_place_factory apply_typed_in_place>`
    が選択されます。

  .. hint::

    この関数は、 ``etude::typed_in_place_factory< T, Args... >``\ :ref:`¶<typed_in_place_factory>`
    の ``apply``\ :ref:`¶<typed_in_place_factory apply>` と
    ``move_apply``\ :ref:`¶<typed_in_place_factory move_apply>` 、および
    Boost.TypedInPlaceFactory の ``apply`` 呼び出しを、
    統一的なインターフェイスで扱うために用意されています。
    
