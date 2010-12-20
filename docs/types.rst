
Type Traits
============

使い道
  様々な型特性メタ関数群


``indices``
-----------

必要ヘッダ
  ::
    
    #include <etude/types/indices.hpp>

定義
  ::
  
    namespace etude {
    
      template<std::size_t... Indices>
      struct indices {
        typedef indices<Indies...> type;
      };
      
      template<std::size_t N>
      struct make_indices
        : indices<0, ..., N-1> {};
      
    }

``etude::indices<Indices...>``\ は、\ ``std::size_t``\ のリストを型に落としこむ為のクラスです。
通常このクラスはライブラリの実装を補助する目的で使われ、表に現れることはありません。

``etude::make_indices<N>``\ は\ ``etude::indices<0..N-1>``\ を継承したクラスで、\
主に\ ``std::tuple``\ をunpackする場合などに、インデックスを生成する目的で使います。

.. hint::

  詳しい使い方は :file:`samples/types/indices.cc` を参照してください。


``decay_and_strip``
-------------------

必要ヘッダ
  ::
    
    #include <etude/types/decay_and_strip.hpp>

定義
  ::
    
    namespace etude {
    
      template<class T>
      struct decay_and_strip {
        typedef see-below type;
      };
      
    }

``etude::decay_and_strip<T>::type``\ は、\ ``std::decay<T>::type``\ を\ ``U``\ としたとき、\
``U``\ が\ ``std::reference_wrapper<X>``\ ならば\ ``X&``\ に、\
そうでなければ\ ``U``\ に定義されます。
