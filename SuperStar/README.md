# ゲーム「SuperStar」開発プロジェクト

-   update: 2024/10/17
-   version: 1.2.0

## ゲーム

| ジャンル             | 描画 |
| -------------------- | ---- |
| シューティングゲーム | 2D   |

## 開発環境

| OS      | バージョン |
| ------- | ---------- |
| Windows | 2022       |

| IDE              | バージョン |
| ---------------- | ---------- |
| VisualStdio 2022 | 17.8.6     |

| 言語 | バージョン |
| ---- | ---------- |
| C++  | 17         |

| ツール | バージョン |
| CMake | 3.30.0 |
| ----- | ------ |

## 利用ライブラリ一覧

| Name                                                                      | Version   |
| ------------------------------------------------------------------------- | --------- |
| [Catch2](https://github.com/catchorg/Catch2)                              | v2.13.10  |
| [DxLib](https://dxlib.xsrv.jp/)                                           | Ver 3.24d |
| [tomlplusplus](https://github.com/marzer/tomlplusplus?tab=readme-ov-file) | V3.4.0    |
| [simdjson](https://github.com/simdjson/simdjson)                          | v3.9.4    |
| [pugixml](https://github.com/simdjson/simdjson)                           | v1.14     |

## 開発目的

-   C++で利用したゲーム開発経験を詰むため
-   エンジン開発経験を詰むため
-   最新技術を試す環境づくり

## VS2022 プロジェクト作成手順

1. ProjBuild.bat ファイルを実行
1. x86 か x64 のどちらかで作成するかの選択画面が出るの選択
1. 作成に成功したら Build ディレクトリが作られていてその中に SuperStar.sln ファイルがあるので開く

## clang-format ツールで c++コード整形の環境構築手順

#### ツール環境構築

1. インストール

    - VisualStudio2022
        - インストール時に自動整形ツール clang-format.exe / clang-tidy.exe が入っているのでツールインストールは不要
        - clang-format.exe がある場所
            - [VisualStudio2022 のインストールフォルダ]\VC\Tools\Llvm\bin\clang-format.exe
        - clang-tidy.exe がある場所
            - [VisualStudio2022 のインストールフォルダ]\VC\Tools\Llvm\bin\clang-tidy.exe
    - CMake
        - VS2022 のプロジェクト作成に使う
        - [インストール先](https://cmake.org/download/)
        - 項目 **Windows x64 Installer:** のインストーラーをダウンロード
        - [参考となるインストール方法](https://qiita.com/matskeng/items/c466c4751e1352f97ce6)

1. 環境変数 CLANG_FORMAT_PATH を追加

    - バッチファイルの一括整形する場合に使う
    - 環境変数には clang-format.exe のパスを絶対パスで設定

1. 環境変数 CLANG_TIDY_PATH を追加
    - バッチファイルの一括整形で利用
    - 環境変数には clang-tidy.exe のパスを絶対パスで設定

#### コード整形内容をファイルに記述

1. プロジェクトのルートディレクトリにある.clang-format ファイルを編集
    - どういった編集をするのかは以下のサイトが参考になる
        - [公式ドキュメント](https://clang.llvm.org/docs/ClangFormatStyleOptions.html)
        - [Clang-Format の書式について](https://qiita.com/koyukitukimino/items/9b8f74d1ae307a7017d3)

#### どういった整形内容になっているか

-   ファイルの改行コードを LF に統一
-   インデントはスペース

## コード整形方法

-   手動

    ※注意点<br>
    サードパーティーのコードを整形してはいけない

    1.  VisualStdio2022 のエディターで整形したいファイルを開く
    1.  ctrl + f でフォーカスしているファイルが整形<br>

-   バッチファイルで一括整形

    1. HobbyEngine/Tool/CodeCleanup.bat を実行

        - 例: CodeClenupSample ディレクトリ内の cpp/h/hpp ファイルを全て自動整形

            ```
            >CodeCleanup.bat CodeClenaupSample\
            ```

            末尾に\をつけないとディレクトリとして認識されないので注意

## 自動整形の非対象ディレクトリ

```
OSS のライブラリなどサードパーティーのコードは ThirdParty ディレクトリに収めて自動整形しないようにしている
```

#### 自動整形の非対象ディレクトリの作成手順

1. ThirdParty ディレクトリを作成
1. 自動整形したくないファイルを ThirdParty ディレクトリに入れる

#### 自動整形にしない理由

1. OSS ライセンスには以下のタイプがある

    - コピーレフト型(FSF,GPL)
    - 準コピーレフト型(MPL)

    ```
    上記タイプのOSSだとソース改変すると公開義務が発生
    自動整形してしまうとソース改変して公開義務が発生してしまう
    なので自動整形しないようにしてソース改変を防いでいる
    ```

2. 自動整形することでコンパイルが通らない可能性をつぶすため

## コーディング規約

### 目的

**コードを読むだけで処理内容を考えずに理解できるようにするため**

### 変数名

変数名を見るだけで変数型を素早く理解するために<br>
ハンガリアン記法を採用<br>
型の記号はわかりやすくかつ変数名を読むのを邪魔にならない程度<br>

-   変数名の先頭につける記号一覧
    |データ型|プレフィックス|説明|
    |----|----|----|
    |Sint8|なし|使う機会がほとんどないから|
    |UInt8|なし|使う機会がほとんどないから|
    |Sint16|is||
    |Uint16|us||
    |Sint32|i||
    |Uint32|u||
    |Sint64|il||
    |Uint64|ul||
    |Float32|f||
    |Float64|d||
    |Bool|b||
    |Char|c||
    |Char[] or Char\* or std::string などの文字列|sz||
    |ポインタ|p|p の前後に型記号はいれない|
    |参照|r||
    |std::shared*ptr|sp||
    |std::unique_ptr*ptr|up||
    |配列 または配列機能を持つクラス|a||
    |std::vector または 同様の機能を持つクラス|v||
    |std::list または 同様の機能を持つクラス|lst||
    |std::map または 同様の機能を持つクラス|m||
    |std::stack または 同様の機能を持つクラス|s||
    |テンプレート|なし||
    |メソッド引数の入力|in\*||
    |値をメソッド出力引数|out||
    |クラスのメンバー変数|\_||
    |構造体のメンバー変数|なし||
    |const|なし|つける記号が増えると逆に見づらくなるので|
    |enum|e||

-   変数名の先頭につける記号のつけ方

    記号は型を付けた左から右の順につける

    -   例 1)
        ```
        // Uintなのでu
        // *でポインターだからp
        // なのでupの記号をつける
        // しかしポインタなのでpのみにする
        Uint32* pCount;
        ```
    -   例 2)
        ```
            class Human
            {
            public:
                // メンバー変数なので_
                // Uint32型なのでuをつける
                Uint32 _uCount = 0;
            };
        ```
    -   例 3)
        ```
        template(class TYPE)
        void Func()
        {
            TYPE _aValue[32];
        }
        ```

### const について

-   関数の戻り値の型が組み込み型の場合は const はつけない
    -   ※ポインタ型の場合は const をつける

### クラスのメソッド名

#### protected or private は外部公開ができない事を読んで判断できるようにするためにメソッド名の先頭に\_をつける

```
    class Object
    {
    private:
        Uint32 _GetIndex() { return 0; }
    };
```

#### 仮想メソッドは V をつける

#### 宣言・定義の両面で見ても仮想メソッドと一目でわかるようにするため

#### 宣言は override で見て一目でわかるが、定義では override が付けれないので一目でわからず考える時間が生まれたから

```
    class Object
    {
    public:
        virtual void VRun() {}
    };
```

### クラスのオーバーライド

```
オーバーライドしたメソッドは基本クラスのメソッドも呼び出す
※ 基本クラスのメソッドの中身が空で呼ぶ必要がないケースもあるが,
   呼ぶか呼ばないかを調べるコストが高くなってしまう

クラスには仮想デストラクタを宣言
※ 基本クラス型のポインタを使って派生クラスのインスタンスを削除した場合,
   派生クラスのデストラクタを必ず呼ぶため

クラスを派生しないなら必ずfinalを使う
```

#### 例)

```
class Object
{
public:
    virtual ~Object() = default;
    virtual void VProcess() {}
};

class ObjectV2 final : public Object
{
public:
    void VProcess() override final { Object::VProcess(); }
};
```

### ファイル名 / ディレクトリ名

#### 大文字のキャメルケース

```
TaskManager.cpp
TaskManager.h
TaskManager.hpp
...
```

### マクロ

-   マクロ名は全て大文字
-   エンジン内のマクロ
    -   プレフィックスは HE\_
-   ゲーム実装内のマクロ
    -   プレフィックスはなし

### enum 名

-   enum 名のプレフィックスに E をつける
-   enum の要素の前に enum 名を付ける
    -   これは enum の名前のかぶりを避けるため
-   enum の要素名を参照する時は enum 名をつけない

    例)

    ```
    enum EState
    {
        EState_Active = 0,
    };

    // OK
    EState e = EState_Active;

    // NG
    EState e1 = EState::EState_Active;
    ```

### クラス内でのメンバー変数の参照

-   クラス内のメンバー変数を参照する時は this をつける

    例)

    ```
    class Object
    {
    public:
        Uint32 Count() const { this->_uCount; }
    private:
        Uint32 _uCount;
    }
    ```

## 注意点

-   git の無視ファイルに Debug ディレクトリを指定しているのでディレクトリ名に Debug を使ってはいけない

# 参考サイト

## Git

-   [Git で管理しているソースの改行コード対応](https://zenn.dev/gekal/articles/git-line-endings-practices)

## CMake

-   [C/C++プロジェクトを CMake でビルドする](https://qiita.com/Hiroya_W/items/049bfb4c6ad3dfe6ff0c)
    -   CMake を使ってプロジェクトを構築方法をシンプルに記載している
    -   初心者におすすめ
-   [CMake 再入門メモ](https://zenn.dev/rjkuro/articles/054dab5b0e4f40)
    -   CMake の基本機能説明をしている
    -   初心者におすすめ
-   [CMake: キャッシュ変数と環境変数](https://qiita.com/mrk_21/items/186439952a6665184444)
    -   グローバル変数のようなもの
    -   複数の CMakeLists.txt から変数を参照できた
-   [Visual Studio 向けに使う CMake](https://hikita12312.hatenablog.com/entry/2018/09/13/101924)

    -   VS のプリコンパイル済みのヘッダーファイルを CMake で作成する方法

-   [Visual Studio ユーザーに送る CMake テクニック ☆ (1)](https://elda27.hatenablog.com/entry/2016/11/29/235301)
    -   VS2022 プロジェクトのフィルタリングを CMake で作成する方法
    -   プロジェクトのファイルを整理して見やすくするために必要

## 参考書籍
