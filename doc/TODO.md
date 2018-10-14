## Prefix rule of commit message
- feat: A new feature
- fix: A bug fix
- docs: Documentation only changes
- style: Changes that do not affect the meaning of the code (white-space, formatting, missing semi-colons, etc)
- refactor: A code change that neither fixes a bug nor adds a feature
- perf: A code change that improves performance
- test: Adding missing or correcting existing tests
- chore: Changes to the build process or auxiliary tools and libraries such as documentation generation

## Coding rule
- Variable names are snake style.
- Function names are camel style.
- Aappend \_t to new name of typedef struct or enum.
- Use type bool of stdbool.h for those that take only binary values such as flag.

## Other rule
- Set file encoding to UTF-8.

## Memo
- autotoolsの導入
- -hvi というようなオプションの指定ができるようにする
- option.cの中身全体的にリファクタリング
- judgeDestination()のコンテンツの数を数える処理はcheckContentsNum()にて行うので不要
- prompt作成したらREADME.mdの更新
- ソースのコメントを英語にする
- 現在はrwh()内でショートカットが入力された際の動作を実装しているが、割り込み処理で実現できないか調べる
- getchは自作しなくてもcurses.hにwgetchが定義されていたのでこっちが使えるかも
- string\_plus.h追加
- datastructure.h追加
