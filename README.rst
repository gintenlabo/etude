=======================
  Etude C++ Libraries
=======================


どんなライブラリ？
-------------------

C++0x によるコーディングに慣れるために書いたコード片を，ライブラリとしてまとめたものです．

主に，標準ライブラリや `Boost C++ Libraries <http://boost.org/>`_ を使用するにあたり，あると便利だと思われるものを収録しています．


ライブラリ一覧
---------------

doc/html/libs.html を見てください．


使い方
-------

本ライブラリはヘッダのみのライブラリです．

使用する際は， ./etude に含まれるヘッダのうち，必要なものを ``#include`` してください．

特に理由がない限り， ``#include <etude/xxx.hpp>`` という形で ``#include`` できるよう，予めインクルードパスを通しておくことを推奨します．


必要条件
---------

本ライブラリは，バージョン 4.5.0 以降の GCC ( -std=c++0x ) で動作確認を行っています．

GCC の独自拡張は使っていないので， GCC 4.5 の C++0x 機能に対応したコンパイラならば，問題なく動く筈です．
（といっても，2011年7月現在， GCC 4.5 と同等の C++0x 対応を行なっているコンパイラは， GCC 以外には存在しませんが…．）

具体的に使っている機能は， http://gcc.gnu.org/gcc-4.5/cxx0x_status.html を参考にしてください．

また，本ライブラリを使うためには， `Boost C++ Libraries <http://boost.org/>`_ のバージョン 1.43.0 以降のヘッダが必須となります． （ビルドする必要はありません．）


ライセンス
-----------

このライブラリは， `Boost Software License <http://www.boost.org/users/license.html>`_ により提供されます．

./LICENSE_1_0.txt または doc/html/license.html を参照してください．


何かあったら
-------------

twitter
  銀天すばる http://twitter.com/SubaruG
blog
  銀天随筆集 http://d.hatena.ne.jp/gintenlabo
email
  gintensubaru あっと gmail.com

辺りで，作者とコンタクトをとることが可能です．


--------------------------------------------

Copyright (C) 2010-11  Takaya Saito (SubaruG)
