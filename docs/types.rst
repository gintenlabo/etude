
Type Traits
============

使い道
  様々な型特性メタ関数群

.. contents::
   :depth: 2
   :local:


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

``etude::decay_and_strip<T>::type`` は、 ``std::decay<T>::type`` を ``U`` としたとき、
``U`` が ``std::reference_wrapper<X>`` ならば ``X&`` に、そうでなければ ``U`` に定義されます。

この型変換は ``std::make_tuple`` において行われる型変換と同じものです。


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
        : indices< 0, ..., N-1 > {};
      
    }

``etude::indices<Indices...>`` は、 ``std::size_t`` のリストを型に落としこむ為のクラスです。
通常、このクラスはライブラリの実装を補助する目的で使われ、表に現れることはありません。

``etude::make_indices<N>`` は ``etude::indices< 0, ..., N-1 >`` を継承したクラスで、\
主に ``std::tuple`` を unpack する場合などに、インデックスを生成する目的で使います。

.. hint::

  詳しい使い方は :file:`samples/types/indices.cc` を参照してください。


``types``
---------

必要ヘッダ
  ::
    
    #include <etude/types/types.hpp>

定義
  ::
  
    namespace etude {
    
      template<class... Types>
      struct types {
        typedef types<Types...> type;
      };
      
    }

``etude::types<Types...>`` は、複数の型に対する単純な type envelope です。
通常、このクラスはライブラリの実装を補助する目的で使われ、表に現れることはありません。

このクラスの典型的な使われ方としては、複数の型を一つに束ねて
``etude::is_convertible`` といったメタ関数に渡す、などが挙げられます。


``is_convertible``
------------------

必要ヘッダ
  ::
    
    #include <etude/types/is_convertible.hpp>

定義
  ::
  
    namespace etude {
    
      template<class From, class To>
      struct is_convertible
        : std::integral_constant<bool, see-below> {};
      
    }

``etude::is_convertible< From, To >`` は、以下のように定義されるメタ関数です：

- ``From`` と ``To`` がそれぞれ ``etude::types<FromTypes...>`` と ``etude::types<ToTypes...>`` である場合

  - ``etude::is_convertible< etude::types<FromTypes...>, etude::types<ToTypes...> >`` は、
    ``FromTypes...`` と ``ToTypes...`` の要素数が共に N で、かつ 0 から N-1 までの全ての整数
    i に対し ``etude::is_convertible< Fi, Ti >::value`` が ``true`` である場合には
    ``std::true_type`` を継承し、そうでなければ ``std::false_type`` を継承します。
    ただし ``Fi`` と ``Ti`` は、 ``FromTypes...`` と ``ToTypes...`` のそれぞれ i 番目の要素です。

- 上記以外で、 ``From`` が ``etude::types<FromTypes...>`` である場合

  - ``etude::is_convertible< etude::types<FromTypes...>, To >`` は、
    ``std::is_constructible< To, FromTypes... >`` を継承します。

- それ以外の場合

  - ``etude::is_convertible< From, To >`` は、 ``std::is_convertible< From, To >`` を継承します。


``is_tuple``
------------

under construction...


``tuple_size``
--------------

under construction...


``tuple_element``
-----------------

under construction...


``tuple_indices``
-----------------

under construction...


``tuple_types``
---------------

under construction...

