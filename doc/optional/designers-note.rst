
設計メモ
========

.. note::
  このドキュメントは，デザイン用のメモ書きです．
  
  リリース版の Etude には含まれません．


基本方針
--------

- 基本的には Boost.Optional に互換

- Move Semantics および emplace に対応

  - Move 周りは C++11 の標準ライブラリに準ずる
  - emplace は InPlaceFactory と タグ付き ctor の双方に対応

- 参照型や単純型に対して最適化する

  - ``T`` が trivially destructible ならば ``optional<T>`` も trivially destructible
  - trivially copyable に関しては， small object ならば考慮したい
  
    - 閾値は ``sizeof(T) <= sizeof(double)`` 程度を考えている
  
  - ``T`` が参照なら生ポインタと同程度のコストになるようにする

- 参照周りの扱いの厳格化

  - ``int i = 0; optional<int&> x = i;`` は ill-formed にし， ``std::ref`` で包む必要があるようにしたい
  
    - ``std::ref`` の問題点は， ``std::bind`` などとの相性が悪い点
    - ``std::ref`` に包まれたものを転送すると，生参照に変換されてしまう恐れがある
    - ``std::ref`` に対応する ``etude::ref`` の導入で解決する予定


実装方針
--------

- ``aligned_storage`` か Unrestricted Unions か

  - 前者は Unrestricted Unions をサポートしていなくても問題ない
  - 後者は ``constexpr`` 対応が行い易く，また細かいルールに抵触しにくい
  - Unrestricted Unions のサポートの有無により切り分ける

- ``constexpr`` 対策はどうするか

  - 可能なら行いたいが，実行時のオーバーヘッドが大きい場合には諦める方針で
