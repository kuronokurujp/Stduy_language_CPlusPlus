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

## clang-formatツールでc++のコードを整形
### 自動整形の手順
### 環境
- VisualStudio2022
    - インストール時に自動整形ツールclang-format.exeが入っているのでツール導入は不要
        - clang-format.exeがある場所
            - [VisualStudio2022のインストールフォルダ]\VC\Tools\Llvm\bin\clang-format.exe
- 環境変数CLANG_FORMAT_PATHを追加
    - バッチファイルで一括整形する場合に使う
    - 環境変数にはclang-format.exeのパスを絶対パスで設定
### 整形内容を記述
1. プロジェクトのルートディレクトリにある.clang-formatファイルを編集
    - どういった編集をするのかは以下のサイトが参考になる
        - [公式ドキュメント](https://clang.llvm.org/docs/ClangFormatStyleOptions.html)
        - [Clang-Formatの書式について](https://qiita.com/koyukitukimino/items/9b8f74d1ae307a7017d3)

### どういった整形内容になっているか
- ファイルの改行コードをLFに統一
- インデントはスペース

### バッチファイルで一括して整形する場合
1. HobbyEngine/Tool/CodeCleanup.batを実行
    - 例: CodeClenupSampleディレクトリ内のcpp/h/hppファイルを全て自動整形

        ```
        >CodeCleanup.bat CodeClenaupSample\
        ````
        末尾に\をつけないとディレクトリとして認識されないので注意
### 自動整形の非対象ディレクトリ
1. ディレクトリに.clang-formatファイルを作成
1. 作成したファイル内容を以下にする
    ```
    ---
    DisbaleFormat: true
    ---
    ```
    こうするとclang-formatはエラーになってファイルの自動整形は失敗する

## 注意点
- gitの無視ファイルにDebugディレクトリを指定しているのでDディレクトリ名にDebugを使ってはいけない

# 利用ライブラリ一覧
Name | Version
--- | ---
[Catch2](https://github.com/catchorg/Catch2) | v2.13.10
[DxLib](https://dxlib.xsrv.jp/) | Ver 3.24d
[tomlplusplus](https://github.com/marzer/tomlplusplus?tab=readme-ov-file) | V3.4.0
[simdjson](https://github.com/simdjson/simdjson) | v3.9.4

