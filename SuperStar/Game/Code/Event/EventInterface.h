#pragma once

#include "Engine/Common/Singleton.h"
#include "Engine/MiniEngine.h"
#include "EventData.h"

namespace InGame::Event
{
    /// <summary>
    /// イベントを受け取り実行するリスナークラス
    /// </summary>
    class EventListenerInterface
    {
    public:
        explicit EventListenerInterface() {}
        virtual ~EventListenerInterface() = default;

        // リスナー名をテキストで返す
        virtual const Char* VName() = 0;

        /// <summary>
        /// リスナーがイベント受け取ったかどうか
        /// </summary>
        virtual const Bool VHandleEvent(EventDataInterfacePtr const&) = 0
        {
            // 受け取ったイベント名を比較して
            // 処理したいイベント名なら処理するようにする。

            // このメソッドは派生クラスで実装する
            // 処理はできるだけ簡素にする。
            // フレームごとに複数のイベントを評価するから
            return TRUE;
        }
    };

    using EventListenerPtr = std::shared_ptr<EventListenerInterface>;

    /// <summary>
    /// イベントマネージャーインターフェイス
    /// </summary>
    class EventManagerInterface
    {
    public:
        enum EConstants
        {
            EConstancs_Infinite = 0xffffffff
        };

        virtual ~EventManagerInterface() = default;

        // 特定のイベント型のハンドラを登録
        virtual const Bool VAddListenr(EventListenerPtr const&, EventTypeStr const&) = 0;

        // 内部テーブルから、リスナーとイベント型のペアを削除
        virtual const Bool VRemoveListener(EventListenerPtr const&, EventTypeStr const&) = 0;

        // 同期的なイベント発信であり、たたぢに処理する
        // 呼び出した瞬間にイベント呼ばれる
        virtual const Bool VTrigger(EventDataInterfacePtr const&) const = 0;

        // 非同期的なイベント発信
        // VTickメソッドでイベントが呼ばれる
        virtual const Bool VQueueEvent(EventDataInterfacePtr const&) = 0;

#if 0
        // 名前の付いたイベント型のうち、次にくるインスタンスを見つける。
        // 送信待ちのｷｭｰから削除
        // bAllOfType = true -> その型のイベントはすべて入力ｷｭｰから削除
        virtual const Bool VAbortEvent(EventTypeStr const&, Bool in_bAllOfType = false) = 0;
#endif

        // ｷｭｰにあるメッセージを処理する
        // 処理待ちメッセージを全て処理したらtrueを返す
        // 違うならfalse
        // キューにあるイベント発信する時間を外部から設定できる
        // 設定した時間によってはキューにあるイベントが全て発信されないこともある。
        virtual const Bool VTick(const Uint32) = 0;

        // イベント型の有効性を確認
        virtual const Bool VValidateType(EventTypeStr const&) const = 0;
    };

}  // namespace InGame::Event
