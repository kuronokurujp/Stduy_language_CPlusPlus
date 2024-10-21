#pragma once

#include "Engine/Common/Singleton.h"
#include "Engine/MiniEngine.h"
#include "EventData.h"

namespace Event
{
    /// <summary>
    /// イベントを受け取り実行するリスナークラス
    /// ※ 継承クラスでメモリ確保はしてはいけない
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
        virtual Bool VHandleEvent(EventDataInterfacePtr const&) = 0
        {
            // 受け取ったイベント名を比較して
            // 処理したいイベント名なら処理するようにする。

            // このメソッドは派生クラスで実装する
            // 処理はできるだけ簡素にする。
            // フレームごとに複数のイベントを評価するから

            // ここで新しいイベント発行はNG
            return TRUE;
        }
    };

    using EventListenerPtr = Core::Memory::SharedPtr<EventListenerInterface>;

}  // namespace Event
