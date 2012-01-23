
設計メモ
========

.. note::
  このドキュメントは，デザイン用のメモ書きです．
  
  リリース版の Etude には含まれません．


基本方針
--------

- 基本的には C++0x 標準の ``<type_traits>`` に準ずる

- integral constant を「返す」ような Traits の場合，以下のような制約を満たす

  - 特に明記されない限り，該当する ``std::integral_constant`` から直接的に派生する
  - 特に明記されない限り，派生先のクラスの本体の定義は空になる
  
  - 与えられた引数から ``value`` を計算できない場合には，原則として ``std::integral_constant`` からは派生せず， ``::value`` も定義されない
  
    - この時， SFINAE を適用できるよう，原則として Traits 内部ではコンパイルエラーを回避する
    - incomplete type に対する扱い等， undefined behavior となるケースでは，その限りではない
    
    - ``::value_type`` や ``::type`` は，必要に応じて定義されるかもしれないし，定義されないかもしれない
    
    - ``::value`` の定義されない型としては， ``etude::no_value_is_defined<T>`` などが考えられ，実装によっては，これらのクラスから派生する形で Traits を定義するかもしれない
    
      - ``etude::no_value_is_defined<T>`` は， ``::value`` こそ定義されないが， ``::value_type`` は ``T`` に， ``::type`` は ``etude::no_value_is_defined<T>`` 自身に それぞれ 定義されたクラスであり， ``::type`` による連鎖を行いたい場合などに使用できる
    
      - ``::value`` の定義されない型としては，この他に ``etude::no_type_is_defined`` も考えられるが，特に明記されない限り， Traits がこれらのクラスから派生している保証は存在しない

- integral constant 以外の ``type`` を「返す」ような Traits の場合，以下のような制約を満たす

  - 与えられた型から ``type`` を計算できない場合（例えば ``common_type<T, U>`` で，該当する型が存在しないような場合）には，原則として ``::type`` は定義されない

    - この時も， SFINAE を適用できるよう，原則として Traits 内部ではコンパイルエラーを回避する
    - incomplete type に対する扱い等， undefined behavior となるケースでは，その限りではない
    
    - そのようなケースでは， Traits は ``etude::no_type_is_defined`` や ``std::enable_if<false>`` といった ``::type`` の定義されないクラスから派生する形で定義されるかもしれないが，特に明記されない限り， Traits がこれらのクラスから派生している保証は存在しない


Traits を計算するときのルール
-----------------------------

- 何らかの整数値をテンプレート引数として受け取りたい場合

  - 直接的にテンプレート引数として受け取る戦略の他に，ある型 ``Type`` を受け取り ``Type::value`` を計算する戦略が考えられる
  - 前者の戦略を使う場合には，原則として，名前の末尾に "_c" を付ける
  - 後者の戦略を使う場合にも，必要ならば "_c" 以外の接尾辞を付けるかもしれない
    
    - 後者の戦略を使う場合，原則として，単純に ``Type::value`` を計算するのではなく， ``etude::make_integral_constant<Type>::value`` を使って計算する
    
      - ``etude::make_integral_constant<Type>::value`` は，以下のように計算される
      
        - ``Type::value`` が存在する場合は， ``Type::value`` になる
        - それ以外のケースで ``Type::type::value`` が存在する場合は， ``Type::type::value`` になる
        - 上記のいずれでもない場合は， ``etude::make_integral_constant<Type>::value`` は定義されない      
      - ``etude::make_integral_constant<Type>::value`` が定義されない場合，該当する Traits の ``::type`` や ``::value`` も定義されない

- 型のシーケンスをテンプレート引数として受け取りたい場合

  - Variadic Templates で直接的にテンプレート引数として受け取る戦略と，ラッパークラスを介して単一の型として受け取る戦略の，二種類が考えられる
  
  - 両者の戦略を併用する
  
  - 後者の戦略を使う場合には，原則として以下のように処理する
    
    - ラッパークラスとしては ``etude::type<Types...>`` を使う
    
    - 直接的に ``etude::type<Types...>`` を受け取るのではなく，一段階はさんで受け取る
    
      - 与えられた型 ``Type`` に対して， ``typename Type::type`` を計算する
      - その型に対して ``etude::type<Types...>`` でパターンマッチを行う
      - ``typename Type::type`` が ``etude::type<Types...>`` でない場合には，該当する Traits の ``::type`` や ``::value`` は定義されない
    
    - Boost.MPL や Boost.Fusion の Random Access Sequence に対しても適用できるとよい
      
      - 今のところ ``mpl_to_types`` や ``fusion_to_types`` といったもので明示的に ``etude::types`` を作るようにすることを考えている（名前は適当， ``make_types_from_mpl_seq`` みたいにするかも）

- クラステンプレートをテンプレート引数として受け取りたい場合

  - ``template< template<class...> class Hoge >`` で受け取るか，あるいはラッパークラスを介して受け取るか
    
    - 後者ならば Boost.MPL と相性が良さそう
    - ラッパークラスを使う場合，どういうデザインにしようか？
    
      - ``Hoge::template apply<Types...>::type`` か ``boost::mpl::apply<Hoge, Types...>::type`` か
      - 直接 Boost に依存しない分，前者を採用するべきか
      - Boost.MPL を使う場合は ``boost::mpl::lambda`` を使えばいいし
      
    - ってか，むしろ全部用意すりゃいいんじゃね？
    - と思ったが，複雑になりすぎるのも厄介なので，やるなら別名前空間に隔離するべきか
    - やるとしたら， Boost.MPL や Boost.Fusion の拡張にするべきか，それとも Instanciations Must Go! のような形にするべきか
    
      - Boost.MPL や Boost.Fusion は， C++0x では Variadic Templates を使えばよく，実装は楽
        
        - テンプレート実引数の最大値は， C++0x では最低でも 1024 個は使えるので， 全部 ``mpl::vector`` で良く， ``mpl::list`` 等を用意する必要はない
        
    - 必要になったら考えることにする

