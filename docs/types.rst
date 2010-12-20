.. index::
  single: types

Type Traits
============

使い道
  様々な型特性メタ関数群

.. contents::
   :depth: 2
   :local:


.. index::
  single: types; decay_and_strip

.. _decay_and_strip:

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


.. index::
  single: types; indices
  single: types; make_indices

.. _indices:

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


.. index::
  single: types; types

.. _types:

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
``etude::is_convertible``\ :ref:`¶<is_convertible>` といったメタ関数に渡す、などが挙げられます。


.. index::
  single: types; is_convertible

.. _is_convertible:

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

- ``From`` と ``To`` がそれぞれ ``etude::types<FromTypes...>``\ :ref:`¶<types>` と
  ``etude::types<ToTypes...>`` である場合

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


.. index::
  single: types; is_tuple

.. _is_tuple:

``is_tuple``
------------

必要ヘッダ
  ::
    
    #include <etude/types/is_tuple.hpp>

定義
  ::
  
    namespace etude {
    
      template<class T>
      struct is_tuple
        : std::integral_constant<bool, see-below> {};
      
    }

``etude::is_tuple<T>`` は、 ``T`` がタプル型なら
（具体的には定数 ``std::tuple_size<T>::value`` が存在し 0 以上なら）
``std::true_type`` を継承し、そうでなければ ``std::false_type`` を継承するメタ関数です。


.. index::
  single: types; tuple_size

.. _tuple_size:

``tuple_size``
--------------

必要ヘッダ
  ::
    
    #include <etude/types/tuple_size.hpp>

定義
  ::
  
    namespace etude {
    
      template<class T>
      struct tuple_size
        : std::tuple_size<typename std::decay<T>::type> {};
      
    }

``etude::tuple_size<T>`` は、 ``T`` にメタ関数 ``std::decay`` を適用することで
CV 修飾子と参照を外した型 ``U`` に対し、 ``std::tuple_size<U>`` を計算するメタ関数です。


.. index::
  single: types; tuple_element

.. _tuple_element:

``tuple_element``
-----------------

必要ヘッダ
  ::
    
    #include <etude/types/tuple_element.hpp>

定義
  ::
  
    namespace etude {
    
      template<std::size_t I, class T>
      struct tuple_element
      {
        typedef see-below type;
      };
      
    }

``etude::tuple_element< I, T >`` は、以下のように定義されたメタ関数です：

- ``T`` が ``const`` または ``volatile`` によって修飾されている場合

  - ``etude::tuple_element< I, U cv >::type`` は
    ``etude::tuple_element< I, U >::type cv`` に定義されます。

- ``T`` が ``U&`` の場合

  - ``etude::tuple_element< I, U& >::type`` は
    ``etude::tuple_element< I, U >::type &`` に定義されます。

- ``T`` が ``U&&`` の場合

  - ``etude::tuple_element< I, U&& >::type`` は
    ``etude::tuple_element< I, U >::type &&`` に定義されます。

- それ以外の場合

  - ``etude::tuple_element< I, T >`` は ``std::tuple_element< I, T >`` を継承します。


.. index::
  single: types; tuple_indices

.. _tuple_indices:

``tuple_indices``
-----------------

必要ヘッダ
  ::
    
    #include <etude/types/tuple_indices.hpp>

定義
  ::
  
    namespace etude {
    
      template<class T>
      struct tuple_indices
        : make_indices<etude::tuple_size<T>::value>::type {};
      
    }

``etude::tuple_indices<T>`` は、（ CV 修飾された）タプル、またはタプルへの参照に対し、\
その全ての要素のインデックスを順に保持した ``etude::indices<Indices...>``\ :ref:`¶<indices>`
を継承します。


.. index::
  single: types; tuple_types

.. _tuple_types:

``tuple_types``
---------------

必要ヘッダ
  ::
    
    #include <etude/types/tuple_types.hpp>

定義
  ::
  
    namespace etude {
    
      template<class T>
      struct tuple_types
        : etude::types<see-below...> {};
      
    }

``etude::tuple_types<T>`` は、（ CV 修飾された）タプル、またはタプルへの参照に対し、\
その全ての要素の型
``etude::tuple_element< 0, T >::type, ... , etude::tuple_element< N-1, T >::type``\
:ref:`¶<tuple_element>`
を順に保持した ``etude::types``\ :ref:`¶<types>` を継承します。
N は ``etude::tuple_size<T>::value``\ :ref:`¶<tuple_size>` です。

.. hint::

  ``T`` が CV 修飾されている場合や参照の場合には、中身の型も同様に修飾されます。

