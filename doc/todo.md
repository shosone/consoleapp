コミット時に対応した番号を記載すること. ここに記載されていない事を行った場合は記載しなくても良い。
### example
FIX [3]: これこれをなになにして修正した  
ENV [10]: これこれを導入した


## ENV(IRONMENT)
- [7] autotoolsの導入

## ADD( FILES OR DIRECTORIES)
- [1] string\_plus.h追加
- [12] datastructure.h追加

## IMP(LEMENT)
- [2] -hvi というようなオプションの指定ができるようにする
- [3] prompt.cのrwiとかその他諸々の実装

## (BUG )FIX
- [11] prompt.cにてバックスペースをsprintf(line, "%s\b \b", line)で実現するとlineが仮に"hoge"となっていてもstrcmp(line, "hoge")が0にならない. 代替方法でこの問題を解決しているが、原因を調べて正しい方法があれば修正すること.
- [13] rwhで長い文字列を入力すると落ちる
- [14] rwhでdeleteキー押下時の動作がおかしい
- [15] rwhでhistory操作時の過去と未来の方向の切り替えがスムーズにいかなくなる何かしらの条件がある

## MOD(FICATION)
- [5] option.cの中身全体的にリファクタリング
- [4] judgeDestination()のコンテンツの数を数える処理はcheckContentsNum()にて行うので不要
- [9] stdbool使う
- [16] prompt作成したらREADME.mdの更新
- [18] ソースのコメントを英語にする

## CON(SIDERATION)
- [20] 現在はrwh()内でショートカットが入力された際の動作を実装しているが、割り込み処理で実現できないか調べる
- [19] getchは自作しなくてもcurses.hにwgetchが定義されていたのでこっちが使えるかも
