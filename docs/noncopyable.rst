.. index:: noncopyable

Noncopyable
============

使い道
  - コピーできないクラスであることを明示する
  - 既存のクラスに対するコピー不能なラッパーを提供する

必要ヘッダ
  ::
    
    #include <etude/noncopyable.hpp>
    #include <etude/immovable.hpp>


概要
-----

`boost::noncopyable <http://www.boost.org/doc/libs/1_45_0/libs/utility/utility.htm#Class_noncopyable>`_ に対し ``= delete;`` や move semantics を対応させ、更に既存のクラスに対するラッパー機能を追加したものです。

copy は不可能だが move は出来る ``etude::noncopyable<T>`` と、 copy も move も不可能な ``etude::immovable<T>`` の二種類が用意されています。

まず基本的な使用法としては、テンプレート引数に何も渡さずに使うことで、 ``boost::noncopyable`` と同様に「 ``private`` 継承させることで、コピー出来ないクラスであることを明示する」ことができます。

boost::noncopyable との違いは、

- move できるか否かを選べる
- ``= delete;`` を使っているのでメタプログラミングに強い
- trivially dafault-constructible である
- trivially destructible である

の四点。

特に「 copy は禁止したいけど、 move は禁止したくない」ケースは、 C++0x でコードを書く上で頻出するので、そのような場合に、簡潔にその意図を示せるクラスがあるのは便利です。

Etude.Noncopyable では更に、 ``etude::noncopyable`` や ``etude::immovable`` の基底クラスを、テンプレートによって自由に指定できるようになっています。 ::

  struct X {};
  
  struct Y
    : etude::noncopyable<X> // etude::noncopyable<X> は X を継承する
  {
    // 実装...
  };

特に、 Boost.Operators のような実装用の空クラスと組み合わせて使い、 ::

  struct Hoge
    : private etude::noncopyable< boost::totally_ordered<Hoge> >
  {
    friend bool operator==( Hoge const& lhs, Hoge const& rhs );
    friend bool operator<( Hoge const& lhs, Hoge const& rhs );
  };

このように書くことで、所謂 Base Class Chaining を行う際の継承順序を柔軟に変更させることが可能になります。

また、 ``etude::noncopyable<T>`` が ``T`` を public 継承する、というのはつまり、 ``etude::noncopyable<T>`` は ``T`` 型と全く同じように扱うことができる、ということです。 ::

  etude::noncopyable<std::vector<int>> vec( { 1, 2, 3 } ); // コンストラクタは explicit
  
  // public 継承しているので、元々のクラスと同じように扱える
  vec.push_back( 4 ); vec.push_back( 5 );
  std::cout << vec[2] << std::endl; // 3

つまり ``etude::noncopyable<T>`` は ``T`` 型を保持する noncopyable なラッパとして使うこともでき、その際に便利なよう、 ``etude::noncopyable<T>`` は ``T`` がクラス以外の場合にも ``T`` 型に対するラッパとして使用できるようになっています。

なお、これらの機能は ``etude::immovable<T>`` でも全く同様に使えます。


使い方
-------

``etude::noncopyable<>`` や ``etude::immovable<>`` を使うには、単純に private 継承すれば問題ありません。 ::

  #include <etude/noncopyable.hpp>
  #include <etude/immovable.hpp>
  
  class hoge
    : private etude::noncopyable<>
  {
    // 実装
    // ...
  };
  class fuga
    : private etude::immovable<>
  {
    // 実装
    // ...
  };
  
  // ...
  
  int main() {
    hoge x;
    // hoge y = x; // コンパイルエラー！
    hoge z = std::move(x); // move は可能
    
    fuga s;
    // fuga t = s;  // コンパイルエラー！
    // fuga u = std::move(s);  // immovable の場合は move も出来ない
  }


public 継承でも「コピーを禁止する」意図からすれば問題はありませんが、多重継承時に共通の基底クラスになる可能性や、またリスコフの置換原則を考えても、 private 継承にしたほうが無難です。

``etude::noncopyable<T>`` や ``etude::immovable<T>`` は、 ``T`` を public 継承したクラスです。 explicit なコンストラクタを介し、 ``T`` のコンストラクタに引数を転送させることが出来ます。 ::

  etude::noncopyable<std::pair<int, std::string>> p( 1, "hoge" );
  // auto p2 = p; // コンパイルエラー！

こちらは private 継承させることで base class chaining に使ったり、あるいは noncopyable なラッパクラスとして使用することが出来ます。 ``T`` はクラス以外であっても構いません： ::

  etude::immovable<int> i( 0 ); // おｋ。
  std::cout << i << std::endl;  // 0


詳細データ
-----------

.. index::
  single: noncopyable; noncopyable

``<etude/noncopyable.hpp>``
~~~~~~~~~~~~~~~~~~~~~~~~~~~

::

  namespace etude {
   namespace noncopyable_ { // ADL 回避
   
    template<class Base = void>
    struct noncopyable
      : public etude::make_derivable<Base>::type
    {
      noncopyable() = default;
      
      noncopyable( noncopyable const& )    = delete;
      void operator=( noncopyable const& ) = delete;
      
      noncopyable( noncopyable&& )            = default;
      noncopyable& operator=( noncopyable&& ) = default;
      
      
      // Base の構築
      
      // Base 自体から構築。ただし explicit
      template< class T = Base,
        class = typename std::enable_if<
          std::is_constructible<Base, T&&>::value
        >::type
      >
      explicit noncopyable( Base && x )
        : base_( std::forward<Base>(x) ) {}
      
      // その他の構築も全て explicit
      template< class... Args,
        class = typename std::enable_if<
          std::is_constructible<Base, Args...>::value
        >::type
      >
      explicit noncopyable( Args&&... args )
        : base_( std::forward<Args>(args)... ) {}
      
    };
   
    template<>
    struct noncopyable<void>
    {
      noncopyable() = default;
      
      noncopyable( noncopyable const& )    = delete;
      void operator=( noncopyable const& ) = delete;
      
      noncopyable( noncopyable&& )            = default;
      noncopyable& operator=( noncopyable&& ) = default;
      
    };
   
   }
   using namespace noncopyable_;
  }

``etude::noncopyable<>`` は、以下の条件を満たす単純な空のクラスです：

- non-copy-constructible
- non-copy-assignable
- move-constructible
- move-assignable
- trivially-default-constructible
- trivially-destructible

通常このクラスは、 private 継承することによりコピー禁止を明示する目的で使われます。

``etude::noncopyable<Base>`` は、以下の条件を満たす、 ``Base`` を public 継承したクラスです：

- non-copy-constructible
- non-copy-assignable
- move-constructible （もし Base が move-constructible ならば）
- move-assignable （もし Base が move-assignable ならば）

ただし ``Base`` がクラスでない場合には、代わりに ``etude::wrapper<Base>`` を継承します。


.. index::
  single: noncopyable; immovable

``<etude/immovable.hpp>``
~~~~~~~~~~~~~~~~~~~~~~~~~

::

  namespace etude {
   namespace immovable_ { // ADL 回避
   
    template<class Base = void>
    struct immovable
      : public etude::make_derivable<Base>::type
    {
      immovable() = default;
      
      immovable( immovable const& )      = delete;
      void operator=( immovable const& ) = delete;
      
      immovable( immovable&& )      = delete;
      void operator=( immovable&& ) = delete;
      
      
      // Base の構築
      
      // Base 自体から構築。ただし explicit
      template< class T = Base,
        class = typename std::enable_if<
          std::is_constructible<Base, T&&>::value
        >::type
      >
      explicit immovable( Base && x )
        : base_( std::forward<Base>(x) ) {}
      
      // その他の構築も全て explicit
      template< class... Args,
        class = typename std::enable_if<
          std::is_constructible<Base, Args...>::value
        >::type
      >
      explicit immovable( Args&&... args )
        : base_( std::forward<Args>(args)... ) {}
      
    };
   
    template<>
    struct immovable<void>
    {
      immovable() = default;
      
      immovable( immovable const& )      = delete;
      void operator=( immovable const& ) = delete;
      
      immovable( immovable&& )      = delete;
      void operator=( immovable&& ) = delete;
      
    };
   
   }
   using namespace immovable_;
  }

``etude::immovable<>`` は、以下の条件を満たす単純な空のクラスです：

- non-copy-constructible
- non-copy-assignable
- non-move-constructible
- non-move-assignable
- trivially-default-constructible
- trivially-destructible

通常このクラスは、 private 継承することによりコピー禁止を明示する目的で使われます。

``etude::immovable<Base>`` は、以下の条件を満たす、 ``Base`` を public 継承したクラスです：

- non-copy-constructible
- non-copy-assignable
- non-move-constructible
- non-move-assignable

ただし ``Base`` がクラスでない場合には、代わりに ``etude::wrapper<Base>`` を継承します。
