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
- Add suffix \_t to new type name of struct and enum.
- Add suffix \_p to variable of out pointer and the case you want to make that attention.
- Use type bool of stdbool.h for those that take only binary values such as flag.
- Add prefix _ to static functions(variables) and functions(variables) which not to be assumed user's use.
- To avoid conflicts, append prefix __ to the arguments of funtions in the header.

## Other rule
- Set source file encoding to UTF-8.

## Memo
- autotoolsの導入
- -hvi というようなオプションの指定ができるようにする
- ソースのコメントを英語にする
- 現在はrwh()内でショートカットが入力された際の動作を実装しているが、割り込み処理で実現できないか調べる
- getchは自作しなくてもcurses.hにwgetchが定義されていたのでこっちが使えるかも
- makeでsampleのビルドをライブラリごとに分ける(ビルド時間短縮のため)
- \_consoleapp_bugno_tの内容もoptionErrno2Msg()で扱うようにする
### Bug
- consoleapp/promptのrwh()を使っていると途中から文字化けする
- 同じオプションを登録したらエラーにする
- regOptProperty()で第三引数をNULLにしたら落ちる
