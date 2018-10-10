## 開発環境

## ファイル追加

## 実装
- [1] sample.cへinteractiveオプション指定時の動作を実装

## 修正
- [2] -hvi というようなオプションの指定ができるようにする
- [3] ./sample -h -i -hでコアダンプになった
- [4] judgeDestination()のコンテンツの数を数える処理はcheckContentsNum()にて行うので不要

## リファクタ
- [5] option.cの中身全体的に

## 検討
- [7] autotoolsの導入
