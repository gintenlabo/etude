
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


<etude/in_place.hpp>
~~~~~~~~~~~~~~~~~~~~

::

  #include "memory/is_in_place_factory.hpp"
  #include "memory/is_typed_in_place_factory.hpp"
  
  #include "memory/in_place_factory.hpp"
  #include "memory/typed_in_place_factory.hpp"
  
  #include "memory/apply_in_place.hpp"

各種ヘッダをインクルードするだけのヘッダです。


<etude/memory/is_in_place_factory.hpp>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

::

  #include <boost/utility/in_place_factory.hpp>

  namespace etude {
  
    using boost::in_place_factory_base;
    
    template<class T> class is_in_place_factory;
    
  }

.. class:: etude::is_in_place_factory<T>
  
  このクラスは、 T が（CV修飾された） InPlaceFactory （ :class:`boost::in_place_factory_base`
  から派生したクラス）の場合、あるいは InPlaceFactory への参照である場合には
  :class:`std::true_type` を継承し、そうでなければ :class:`std::false_type` を継承します。
  
  .. hint::
    
    特別な理由がなければ、このメタ関数を直接使うのではなく、
    
    代わりに :file:`<etude/memory/apply_in_place.hpp>` で定義されたメタ関数
    :class:`etude::is_in_place_applyable` を使う方がよいでしょう。


<etude/memory/in_place_factory.hpp>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

::

  #include "is_in_place_factory.hpp"
  #include <tuple>

  namespace etude {
    
    // class template in_place_factory
    template<class... Args>
    class in_place_factory;
    
    // in_place_factory non-member functions
    template<class... Args>
    inline std::tuple<Args> const& get_tuple( in_place_factory<Args...> const& x );
    template<class... Args>
    inline std::tuple<Args>&& get_tuple( in_place_factory<Args...> && x );
    
    template<class T, class... Args>
    inline T* apply_in_place( in_place_factory<Args...>& x, void* addr );
    template<class T, class... Args>
    inline T* apply_in_place( in_place_factory<Args...> const& x, void* addr );
    template<class T, class... Args>
    inline T* apply_in_place( in_place_factory<Args...> && x, void* addr );
    
    // function template in_place
    template<class... Args>
    inline in_place_factory<Args&&...> in_place( Args&& ...args );
    template<class... Args>
    inline in_place_factory<Args...> in_place_by_ref( Args&& ...args );
    template<class... Args>
    inline in_place_factory<VArgs...> in_place_by_val( Args&& ...args );
    template<class... Args>
    inline in_place_factory<Args...> in_place_from_tuple( std::tuple<Args...> const& );
    template<class... Args>
    inline in_place_factory<Args...> in_place_from_tuple( std::tuple<Args...> && );
    
  }

.. class:: etude::in_place_factory<Args>

  :func:`etude::in_place` の結果を保持するためのクラス。
  
  このクラスは InPlaceFactory の要件を満たします。
  
  ::
  
    template<class... Args>
    struct in_place_factory
      : boost::in_place_factory_base
    {
      typedef std::tuple<Args...> tuple_type;
      
      // construct, copy
      explicit in_place_factory( Args&& ...args );
      
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
      
     private:
      tuple_type x; // exposition only
    };

  
  .. type:: etude::in_place_factory::tuple_type
    
    内部に保持する引数パックの型です。
    
    :class:`in_place_factory` は、内部にこの型のメンバを一つだけ保持します。
  
  .. function:: explicit etude::in_place_factory( Args&& args )
  
    与えられた引数を保持する :class:`in_place_factory\<Args\>` を構築します。
  
  .. function:: etude::in_place_factory( tuple_type const& ), etude::in_place_factory( tuple_type && )
  
    :type:`tuple_type` に格納された引数リストから :class:`in_place_factory\<Args\>` を構築します。
    
    .. note::
      
      このコンストラクタは都合により、任意のタプルから構築できるようには実装されていません。
      
      言語仕様上、ユーザ定義の型変換は一度しか行われないため、
      このコンストラクタに渡すタプルの型が :type:`std::tuple\<Args...\>` と厳密に一致しない場合、
      たとえタプルの中身が変換可能であっても、コンパイルエラーとなります。
      
      具体的な例を挙げると、 ::
      
        etude::in_place_factory<double> x = std::make_tuple(1);
      
      は、 ``std::make_tuple(1)`` の結果である :type:`std::tuple\<int\>` と、
      :type:`etude::in_place_factory\<double\>` のコンストラクタが要求する :type:`std::tuple<double>`
      の型が厳密に一致しないため、 ill-formed です。
      
      型の厳密に一致しないタプルから :class:`in_place_factory\<Args\>` を構築したい場合は、
      :func:`in_place_from_tuple` を用いて、一度 該当する型の :class:`in_place_factory\<Args\>` に変換してください。
  
  ``template<class... Types> in_place_factory( in_place_factory<Types...> const& );`` ``template<class... Types> in_place_factory( in_place_factory<Types...> && );``
    異なる型の引数を保持する ``in_place_factory`` からの型変換を提供します。
    
    std::tuple<Args...> が std::tuple<Types...> から構築可能でなければいけません。
    
    .. hint::
    
      主に ::
      
        etude::in_place_factory<double> x = etude::in_place( 0 );
      
      のように使います。
  
  ``template<class T> T* apply( void* addr ) const;``
    コンストラクタで渡された引数を用いて、指定されたメモリ領域に T 型のオブジェクトを構築し、
    構築されたオブジェクトへのポインタを返します。
    
    この関数は、以下のコードと同等です： ::
    
      ::new (addr) T( args... )
      
    ただし args... はコンストラクタで指定された引数で、 const 参照として渡されます。
    
    .. warning::
      
      この関数はアライメントや例外安全などに問題のある、本質的に安全でない関数であり、
      安易な呼び出しは未定義動作を容易に引き起こします。
      
      上記の説明の意味が分からない場合は、決してこの関数を自分で呼び出してはいけません。
  
  ``template<class T> T* move_apply( void* addr );``
    コンストラクタで渡された引数を用いて、指定されたメモリ領域に T 型のオブジェクトを構築し、
    構築されたオブジェクトへのポインタを返します。
    
    その際、この関数は、格納している引数を move します。
    
    この関数は、以下のコードと同等です： ::
    
      ::new (addr) T( std::forward<Args>(args)... )
      
    ただし args... はコンストラクタで指定された引数で、右辺値参照として渡されます。
    
    .. note::
    
      この関数が呼び出されると、内部に格納された変数は move され、有効な状態ではなくなります。
      
      これはつまり、この関数の呼び出し後は、このクラスに対するあらゆる操作は
      undefined behavior を引き起こす可能性がある、ということです。
    
    .. warning::
      
      この関数はアライメントや例外安全などに問題のある、本質的に安全でない関数であり、
      安易な呼び出しは未定義動作を容易に引き起こします。
      
      上記の説明の意味が分からない場合は、決してこの関数を自分で呼び出してはいけません。

  ``tuple_type const& get_tuple() const;``
    コンストラクタで渡された引数を格納したタプルへの const 参照を得ます。
    
  
  ``tuple_type const& move_tuple() const;``
    コンストラクタで渡された引数を格納したタプルを move します。
    
    .. note::
    
      この関数が呼び出されると、内部に格納された変数は move され、有効な状態ではなくなります。
      
      これはつまり、この関数の呼び出し後は、このクラスに対するあらゆる操作は
      undefined behavior を引き起こす可能性がある、ということです。

``in_place_factory`` non-member functions
  ``template<class... Args> inline std::tuple<Args> const& get_tuple( in_place_factory<Args...> const& x );``
    ::
    
      x.get_tuple()
    
    と同じです。
  
  ``template<class... Args> inline std::tuple<Args> const& get_tuple( in_place_factory<Args...> && x );``
    ::
    
      x.move_tuple()
    
    と同じです。
  
  ``template<class T, class... Args> inline T* apply_in_place( in_place_factory<Args...>& x, void* addr );`` ``template<class T, class... Args> inline T* apply_in_place( in_place_factory<Args...> const& x, void* addr );``
    ::
    
      x.template apply<T>( addr )
    
    と同じです。
    
  ``template<class T, class... Args> inline T* apply_in_place( in_place_factory<Args...> && x, void* addr );``
    ::
    
      x.template move_apply<T>( addr )
    
    と同じです。
  
  .. hint::
    
    これらの関数は、対象オブジェクトが lvalue か rvalue かによって呼び出す関数を切り替える、
    といった面倒な手間をなくすために提供されています。
    
function template ``in_place``
  ``template<class... Args> inline in_place_factory<Args&&...> in_place( Args&& ...args );``
    与えられた引数への右辺値参照を束縛した ``in_place_factory`` を構築します。
    
    この関数は与えられた引数を「そのままに」束縛します。
    
    これはつまり、 ::
    
      etude::apply_in_place<T>( etude::in_place( args... ), addr )
    
    と、 ::
    
      ::new(addr) T( args... )
    
    が、コンパイラの最適化に依らず、意味論的に同じ動作をする、ということです。
    
    この動作は、速度と意味論の双方において、通常は極めて望ましいものですが、
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
    
    これらの問題を避けてより安全に使いたい場合は、 ``in_place_by_val`` を使ってください。
    
    .. note::
    
      引数を与えず単に ``etude::in_place()`` と使う場合には、
      変数への束縛もコピーも、安全に行うことが出来ます。
      
      その場合は、わざわざ ``etude::in_place_by_val()`` と書く必要はありません。
      
  ``template<class... Args> inline in_place_factory<Args...> in_place_by_ref( Args&& ...args );``
    与えられた引数を束縛した ``in_place_factory`` を構築します。
    
    この関数は ``in_place`` とは違い、一時オブジェクトを値として束縛するので、
    変数に格納しても安全に使うことができますし、コピーも通常のクラスと同じように行えます。
    
    一方で、一時オブジェクト以外のオブジェクト（lvalue）は参照として束縛されるので、
    関数の戻り値として、この関数の結果を使うことは出来ません。
    
    また、参照として束縛されるため、参照先のオブジェクトが変更された場合、
    意図しない結果になることもあります： ::
    
      int i = 0;
      auto x = etude::in_place_by_ref(i);
      i = 42; // x の「中身」は 42 になる
    
    この問題を避けてより安全に使いたい場合は、 ``in_place_by_val`` を使ってください。

  ``template<class... Args> inline in_place_factory<VArgs...> in_place_by_val( Args&& ...args );``
    与えられた引数を値として束縛した ``in_place_factory`` を構築します。
    
    この関数は、以下の関数呼び出しと等価です： ::
    
      etude::in_place_from_tuple( std::make_tuple( std::forward<Args>(args)... ) )
    
    与えられた引数は全て値として（配列や関数の場合はポインタとして）束縛され、
    参照として束縛させたい場合は ``std::ref`` や ``std::cref`` を使って渡します。
    
    この関数は本質的に安全ですが、一方で原則的に全ての引数を値によりキャプチャするため、
    コンパイラにより、特にコピーコストの高いオブジェクトに関しては、非効率的になる場合があります。
    
    .. note::
    
      ``in_place``, ``in_place_by_ref``, ``in_place_by_val`` の関数群は、現状
      あまり良い名前とは言えないため、
      より相応しい名前が見つかった場合には変更する可能性があります。

  ``template<class... Args> inline in_place_factory<Args...> in_place_from_tuple( std::tuple<Args...> const& );`` ``template<class... Args> inline in_place_factory<Args...> in_place_from_tuple( std::tuple<Args...> && );``
    引数をパックしたタプルから ``in_place_factory`` を構築します。
    
    .. hint::
    
      この関数は、コンストラクタ呼び出しに関し、擬似的なタプルの unpack として機能します。
      
      つまり、 ::
      
        etude::apply_in_place<T>( etude::in_place_from_tuple(t), addr );
      
      は、あたかも ::
      
        ::new(addr) T( unpack(t) );
      
      のように動作します。


<etude/memory/is_typed_in_place_factory.hpp>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

::

  #include <boost/utility/typed_in_place_factory.hpp>

  namespace etude {
  
    using boost::typed_in_place_factory_base;
    
    template<class T> struct is_typed_in_place_factory;
    template<class T> struct typed_in_place_associated;
  }

``template<class T> struct is_typed_in_place_factory;``
  T が（CV修飾された） TypedInPlaceFactory （ ``boost::is_typed_in_place_factory``
  から派生したクラス）の場合、あるいは TypedInPlaceFactory への参照である場合には
  ``std::true_type`` を継承し、そうでなければ ``std::false_type`` を継承したメタ関数です。

``template<class T> struct typed_in_place_associated;``
  T が（CV修飾された） TypedInPlaceFactory 、あるいはその参照の場合には、
  ``typename typed_in_place_associated<T>::type`` は
  ``typename TypedInPlaceFactory::value_type`` に定義されます。
  
  そうでなければ typed_in_place_associated<T>::type は定義されません。
  
  .. hint::
    
    このメタ関数は、デフォルト関数テンプレート引数を用いて ::
    
      template<class TypedInPlace,
        class T = typename etude::typed_in_place_associated<TypedInPlace>::type>
      result function( TypedInPlace && x, ～ );
    
    のように使われることを想定しています。


