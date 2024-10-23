#pragma once

#include <functional>

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

    /// <summary>
    /// イベント関数を登録できるリスナー
    /// </summary>
    class EventListenerWithRegistEventFunc : public Event::EventListenerInterface
    {
        HE_CLASS_COPY_NG(EventListenerWithRegistEventFunc);
        HE_CLASS_MOVE_NG(EventListenerWithRegistEventFunc);

    public:
        EventListenerWithRegistEventFunc(
            const Core::Common::FixString128& in_szrName,
            std::function<Bool(Event::EventDataInterfacePtr const&)> in_func)
        {
            this->_func   = in_func;
            this->_szName = in_szrName;
        }

        virtual ~EventListenerWithRegistEventFunc() { this->_func = NULL; }

        // リスナー名
        const Char* VName() { return this->_szName.Str(); }

        /// <summary>
        /// リスナーがイベント受け取ったかどうか
        /// </summary>
        Bool VHandleEvent(Event::EventDataInterfacePtr const& in_spEventData) override final
        {
            HE_ASSERT(this->_func != NULL);
            return this->_func(in_spEventData);
        }

    private:
        std::function<Bool(Event::EventDataInterfacePtr const&)> _func;
        Core::Common::FixString128 _szName;
    };

    using EventListenerPtr = Core::Memory::SharedPtr<EventListenerInterface>;

}  // namespace Event
