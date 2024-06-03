# シューティングゲーム「SuperStar」の開発プロジェクトのリソース

## ゲーム
ジャンル | 描画
--- | ---
シューティングゲーム | 2D

## 開発環境
OS | Version
--- | ---
Windows | 2022
<br>
IDE | Version
--- | ---
VisualStdio 2022 | 17.8.6

## 開発目的
- C++で利用したゲーム開発経験値をあげるため

## 注意点
- gitの無視ファイルにDebugディレクトリを指定しているのでDディレクトリ名にDebugを使ってはいけない

## メモ
- VS2022のエディター動作が重い
- VS2022のデバッグモード起動して終了すると例外エラーが出る事がある
- VS2022のコードファイルの文字コードはUTF-8にしないと警告 / エラーが出る

## 参考
- [VisualStudioのプリコンパイル設定](https://usagi.hatenablog.jp/entry/2019/12/08/073000)
    - プロジェクトへ強制インクルードする方法を取る事でコードでインクルードする必要がない
- [VisualStudioでのGoogleTestを使った単体テスト方法](https://learn.microsoft.com/ja-jp/visualstudio/test/writing-unit-tests-for-c-cpp?view=vs-2022)
    - テストするコードは*.hのみではなく*.cppもインクルードする
    - 最初は別プロジェクト参照してobjを含めてテストしようと思ったが,うまくいかなった
- [Catch2というシングルヘッダーで単体テストできるOSSをVSに導入](https://qiita.com/AngelCase/items/019a8a208608fe9e3161)
- [Catch2の導入方法](https://engineering.nature.global/entry/blog-fes-2022-catch2)
    - curlコマンドでCatch2のヘッダーファイル取得する方法があったが, githubページに[download](https://github.com/catchorg/Catch2/tree/v2.x?tab=readme-ov-file)があったので,ここから取得するのがいい
- [VisualStudioのコードファイル文字コードをUTF-8のBOM付きにする](https://gothlab.org/warning-c4819/)

