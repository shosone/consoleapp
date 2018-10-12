コミット時に対応した番号を記載すること. ここに記載されていない事を行った場合は記載しなくても良い。
### example
FIX [3]: これこれをなになにして修正した  
ENV [10]: これこれを導入した


## ENV(IRONMENT)
- [7] autotoolsの導入

## ADD( FILES OR DIRECTORIES)

## IMP(LEMENT)
- [2] -hvi というようなオプションの指定ができるようにする
- [3] prompt.cのrwiとかその他諸々の実装

## (BUG )FIX
- [11] prompt.cにてバックスペースをsprintf(line, "%s\b \b", line)で実現するとlineが仮に"hoge"となっていてもstrcmp(line, "hoge")が0にならない. 代替方法でこの問題を解決しているが、原因を調べて正しい方法があれば修正すること.

## MOD(FICATION)
- [5] option.cの中身全体的に
- [4] judgeDestination()のコンテンツの数を数える処理はcheckContentsNum()にて行うので不要

## CON(SIDERATION)
