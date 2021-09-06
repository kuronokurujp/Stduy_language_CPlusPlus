## 開発プログラム
    リバーシをプレイする事ができるプログラムです。
    黒石をプレイヤー・白石がAIです。
    AIはMiniMaxの探索アルゴリズムで打つ手を決めます。
    描画はレイトレーシングで行っています。

<img width="435" alt="GameScreen" src="https://user-images.githubusercontent.com/6584759/119507655-fad02600-bda9-11eb-8a8b-d2f64fce7fe2.png">

## 開発環境
    OS:            Windows10
    Visual Studio: Visual Studio Community 2017

## 開発目的
    リアルタイムレイトレーシングで
    フォトレンダリングの品質が高い見た目でゲームが作れるかの検証をするために作りました。

## うまくいかなかったこと

1. レイトレーシングを利用しているが品質の高い見た目に出来なかった

1. 描画負荷が高いのを解消出来なかった

1. FreeType2を利用してテキストフォント描画しているが、<br>
    うまく使えてなくてフォントが汚くて見えにくくなっている

## ゲーム操作

1. 指し手がプレイヤーになった時にプレイヤーが置ける石が灰色になって表示されます。

    <img width="434" alt="GamePlay" src="https://user-images.githubusercontent.com/6584759/119507717-0a4f6f00-bdaa-11eb-8fc9-95b30359790f.png">

1. 灰色になっている石をマウスクリックするとクリックした石が置かれます

1. 敵側に切り替わりAIで判断して石を起きます。

1. 1に戻ります

## 3rd party libraries

- フォント描画 
    - <a href="https://www.freetype.org/">FreeType2</a>
