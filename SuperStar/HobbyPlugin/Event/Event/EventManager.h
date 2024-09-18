#pragma once

#include <list>
#include <set>

#include "Engine/Common/CustomMap.h"
#include "Engine/Common/CustomVector.h"
#include "Engine/MiniEngine.h"
#include "EventInterface.h"

namespace Event
{
    using EventListenerList = std::vector<EventListenerPtr>;
    using EventTypeStrList  = Core::Common::CustomFixVector<EventTypeStr, 256>;

    /// <summary>
    /// イベント管理のストラテジーインターフェイス
    /// </summary>
    class EventManagerStrategyInterface
    {
    public:
        EventManagerStrategyInterface()                               = default;
        virtual ~EventManagerStrategyInterface()                      = default;
        virtual Bool VIsEventTypeStr(const EventTypeStr& in_rTypeStr) = 0;
    };

    /// <summary>
    /// イベント管理
    /// </summary>
    class EventManager final : public EventManagerInterface
    {
    public:
        EventManager(Core::Memory::UniquePtr<EventManagerStrategyInterface>);
        virtual ~EventManager();

        /// <summary>
        /// イベントがないかどうか
        /// </summary>
        Bool EmptyEvent() const;

        // 手動解放
        void VRelease() override final;

        /// <summary>
        /// リスナー登録
        /// 登録したらTRUEを返す
        /// すでに登録済みなど登録失敗したらFALSE
        /// </summary>
        Bool VAddListenr(EventListenerPtr const&, EventTypeStr const&) override final;

        Bool VRemoveListener(EventListenerPtr const&, EventTypeStr const&) override final;

        Bool VTrigger(EventDataInterfacePtr const&) const override final;

        Bool VQueueEvent(EventDataInterfacePtr const&) override final;

#if 0
        Bool VAbortEvent(EventTypeStr const&) override final;
#endif

        Bool VTick(const Uint32) override final;

        Bool VValidateType(EventTypeStr const&) const override final;

        // 情報探索メソッド

        // 特定のイベント型に関連づけられたリスナーのリストを取得
        Bool OutputListenerList(EventListenerList*, EventTypeStr const&) const;

    private:
        // TODO: stdのデータ構造はすべて自前で作成したカスタム用に差し替える予定

        // グローバルインスタンスを一つ定義
        using EventTypeSet = std::set<Uint64>;
        // イベント型のセットに結果を導入
        using EventTypeSetRes = std::pair<EventTypeSet::iterator, Bool>;
        // イベント型ごとに一つのリスト(マップに格納される)
        using EventListenerTable = std::list<EventListenerPtr>;
        // イベント識別子をリスナーリストにマッピング
        using EventListenerMap = Core::Common::CustomFixMap<Uint64, EventListenerTable, 512>;
        // イベントリスナーマップのエントリ
        using EventListenerMapEnt = std::pair<Uint32, EventListenerTable>;
        // 処理待ちイベントのキュー、または処理中のイベントのキュー
        using EventQueue = std::list<EventDataInterfacePtr>;

        enum EConstants
        {
            EConstants_NumQueues = 2
        };

        // イベント型をリスナーにマッピング
        EventListenerMap _mRegistry;

        // 二重バッファのイベント処理のキュー
        EventQueue _aQueue[EConstants_NumQueues];

        // どのキューがアクティブ処理されているか？
        // キューに入ろうとするイベントは他方のキューに置かれる
        Sint32 _sActiveQueue = 0;

        Core::Memory::UniquePtr<EventManagerStrategyInterface> _upStrategy;
    };

}  // namespace Event
