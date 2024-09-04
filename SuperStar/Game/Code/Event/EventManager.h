#pragma once

#include <list>
#include <set>

#include "Engine/Common/CustomMap.h"
#include "Engine/Common/CustomVector.h"
#include "Engine/MiniEngine.h"
#include "EventInterface.h"

namespace InGame::Event
{
    using EventListenerList = std::vector<EventListenerPtr>;
    using EventTypeStrList  = Core::Common::CustomFixVector<EventTypeStr, 256>;

    /// <summary>
    /// イベント管理のストラテジーインターフェイス
    /// </summary>
    class EventManagerStrategyInterface
    {
    public:
        virtual ~EventManagerStrategyInterface()                            = default;
        virtual const Bool VIsEventTypeStr(const EventTypeStr& in_rTypeStr) = 0;
    };

    class EventManager final : public EventManagerInterface,
                               public Core::Common::Singleton<EventManager>
    {
    public:
        EventManager(std::unique_ptr<EventManagerStrategyInterface>);
        virtual ~EventManager();

        /// <summary>
        /// イベントがないかどうか
        /// </summary>
        const Bool EmptyEvent() const;

        /// <summary>
        /// インスタンスがリリースされたときに呼ばれる
        /// 終了処理だが, 大抵はアプリ終了時に呼ばれる
        /// </summary>
        const Bool VRelease() override final;

        /// <summary>
        /// リスナー登録
        /// 登録したらTRUEを返す
        /// すでに登録済みなど登録失敗したらFALSE
        /// </summary>
        const Bool VAddListenr(EventListenerPtr const&, EventTypeStr const&) override final;

        const Bool VRemoveListener(EventListenerPtr const&, EventTypeStr const&) override final;

        const Bool VTrigger(EventDataInterfacePtr const&) const override final;

        const Bool VQueueEvent(EventDataInterfacePtr const&) override final;

#if 0
        const Bool VAbortEvent(EventTypeStr const&) override final;
#endif

        const Bool VTick(const Uint32) override final;

        const Bool VValidateType(EventTypeStr const&) const override final;

        // 情報探索メソッド

        // 特定のイベント型に関連づけられたリスナーのリストを取得
        const Bool OutputListenerList(EventListenerList*, EventTypeStr const&) const;

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

        std::unique_ptr<EventManagerStrategyInterface> _pStrategy;
    };

    // イベントマネージャーメソッドのヘルパー関数
    const Bool SafeAddListener(EventListenerPtr const&, EventTypeStr const&);
    const Bool SafeRemoveListener(EventListenerPtr const&, EventTypeStr const&);
    const Bool SafeTriggerEvent(EventDataInterfacePtr const&);
    const Bool SafeQueueEvent(EventDataInterfacePtr const&);
#if 0
    const Bool SafeAbortEvent(EventTypeStr const&, const Bool in_bAllOfType);
#endif
    const Bool SafeTickEventManager(const Uint32 in_uMaxMillis);
    // イベント型の有効性を確認
    const Bool SafeValidateEventType(EventTypeStr const&);

}  // namespace InGame::Event
