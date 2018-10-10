## 開発環境

## ファイル追加

## 実装
- sample.cへinteractiveオプション指定時の動作を実装

## 修正
- --help=aでaがhelpのコンテンツとして扱われない
- -hvi というようなオプションの指定ができるようにする
- ./sample -h -i -hでコアダンプになった
- judgeDestination()のコンテンツの数を数える処理はcheckContentsNum()にて行うので不要

## リファクタ
- option.cの中身全体的に
- readlineWithHistory

## 検討
- autotoolsの導入
