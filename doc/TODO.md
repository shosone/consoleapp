## ENV(IRONMENT)
- autotoolsの導入

## ADD( FILES OR DIRECTORIES)

## IMP(LEMENT)
- -hvi というようなオプションの指定ができるようにする

## (BUG )FIX
- rwh()でdeleteキー押下時の動作がおかしい

## MOD(FICATION)
- option.cの中身全体的にリファクタリング
- judgeDestination()のコンテンツの数を数える処理はcheckContentsNum()にて行うので不要
- prompt作成したらREADME.mdの更新
- ソースのコメントを英語にする

## CON(SIDERATION)
- 現在はrwh()内でショートカットが入力された際の動作を実装しているが、割り込み処理で実現できないか調べる
- getchは自作しなくてもcurses.hにwgetchが定義されていたのでこっちが使えるかも
- string\_plus.h追加
- datastructure.h追加
