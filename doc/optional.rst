.. index:: optional

Optional
========

使い道
  - ``T`` 型かもしれないし，そうではないかもしれない型 ``optional<T>`` を提供する

必要ヘッダ
  ::
    
    #include <etude/optional.hpp>


概要
-----

`boost::optional <http://www.boost.org/doc/libs/1_48_0/libs/optional/doc/html/index.html>`_
に対して move semantics を導入し，更に参照や簡単なクラスに対する最適化を行ったものです．

最終的には Boost へのフィードバック，更に標準ライブラリでの採用を目指しています．


使い方
-------

基本的な使い方は， Boost.Optional と同一です． ::

  #include <iostream>
  #include <etude/optional.hpp>
  
  // 関数の戻り値として
  etude::optional<std::string> getline_optinal( std::istream& is )
  {
    std::string buf;
    if( std::getline( is, buf ) ) {
      return std::move(buf);
    }
    else {
      return boost::none;
    }
  }
  
  int main()
  {
    while( auto line = getline_optional(std::cin) ) {
      std::cout << *line << std::endl;
    }
  }

:doc:`InPlaceFactory<in-place>` による初期化も， Boost.Optional と同様に備えています．


また， Etude.Optional に特有の機能として， ``emplace_construct`` による直接初期化があります．
これは C++11 の STL における ``emplace`` 系列と同じく，与えられた引数から直接コンストラクタを呼ぶものです． ::

  #include <vector>
  #include <etude/optional.hpp>
  
  int main()
  {
    // *x を vector<int>( 10, 0 ) で直接初期化
    etude::optional<std::vector<int>> x( etude::emplace_construct, 10, 0 );
    
    // ～
    
  }


詳細データ
-----------

under construction.

