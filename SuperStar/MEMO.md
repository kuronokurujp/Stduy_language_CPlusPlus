# 開発時に気づいたことや気になったことなどのメモ

## 開発環境
### VS2022にインストールしたプラグインVsVimの動作が重い
- コピー処理が重い
- 2019のバージョンではこの症状はでなかった
- PCによってはこの症状は起きなかった

## 開発ルール
- エンジンのプラグインはすべてモジュールヘッダーファイルをインクルードしたら使えるようにする
    - simdjsonなど一つのhファイルをインクルードすると使えるシンプルな仕組みが好きでマネた

## コーディングルール
### 関数型のマクロを使う時は;を付けるのがいい
- C++の関数呼び出しの末尾に;をつけるので合わせた方がいいと思った
- C++の書き方に揃えたいから

## エラー
### VS2022のデバッグモード起動して終了すると例外エラーが出る事がある
- exit.cpp
    - exit_or_terminate_process関数のExitProcess関数で例外エラー
    - ライブラリDxLibなのかも
### VS2022のコードファイルの文字コードはUTF-8にしないと警告 / エラーが出る
- BOMなしでも警告やエラーは出なくなった

## ライブラリ
### Jsonライブラリの導入
Name | Version | wchar_t型が対応 | C++17 | 採用 | 採用理由
--- | --- | --- | --- | --- | ---
RapidJson | 1.1.0 | している | 対応が不十分 | 不採用 | C++17対応が不十分だったから, \n tomlplusplusというライブラリを組み合わせるとエラーになった \n 最新バージョンが2016年と古い
Simdjson | 3.9.4 | していない | 対応が十分 | 採用 | 要素取得が高速で1/60秒内での取得が可能と判断
nlohman/json | 3.11.3 | していない | 対応が十分 | 不採用 | 要素取得が低速で1/60秒内での取得ができないと判断

## 参考サイト
- [VisualStudioのプリコンパイル設定](https://usagi.hatenablog.jp/entry/2019/12/08/073000)
    - プロジェクトへ強制インクルードする方法を取る事でコードでインクルードする必要がない
- [VisualStudioでのGoogleTestを使った単体テスト方法](https://learn.microsoft.com/ja-jp/visualstudio/test/writing-unit-tests-for-c-cpp?view=vs-2022)
    - テストするコードは*.hのみではなく*.cppもインクルードする
    - 最初は別プロジェクト参照してobjを含めてテストしようと思ったが,うまくいかなった
- [Catch2というシングルヘッダーで単体テストできるOSSをVSに導入](https://qiita.com/AngelCase/items/019a8a208608fe9e3161)
- [Catch2の導入方法](https://engineering.nature.global/entry/blog-fes-2022-catch2)
    - curlコマンドでCatch2のヘッダーファイル取得する方法があったが, githubページに[download](https://github.com/catchorg/Catch2/tree/v2.x?tab=readme-ov-file)があったので,ここから取得するのがいい
- [VisualStudioのコードファイル文字コードをUTF-8のBOM付きにする](https://gothlab.org/warning-c4819/)

 
