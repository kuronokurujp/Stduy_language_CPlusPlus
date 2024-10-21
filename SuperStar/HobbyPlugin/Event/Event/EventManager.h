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
        EventManagerStrategyInterface()             = default;
        virtual ~EventManagerStrategyInterface()    = default;
        virtual Bool VIsEventTypeHash(const Uint64) = 0;
    };

    /// <summary>
    /// イベント管理
    /// 複数のイベントタイプを扱える
    /// </summary>
    class EventManager final
    {
    public:
        enum EConstants
        {
            EConstants_NumQueues = 2,
            EConstancs_Infinite  = 0xffffffff
        };

    public:
        EventManager(Core::Memory::UniquePtr<EventManagerStrategyInterface>);
        virtual ~EventManager();

        /// <summary>
        /// イベントがないかどうか
        /// </summary>
        Bool EmptyEvent() const;

        // 手動解放
        void Release();

        /// <summary>
        /// リスナー登録
        /// 登録したらTRUEを返す
        /// すでに登録済みなど登録失敗したらFALSE
        /// </summary>
        Bool AddListenr(EventListenerPtr const&, EventTypeStr const&);

        Bool RemoveListener(EventListenerPtr const&, EventTypeStr const&);
        Bool RemoveAllListener(EventTypeStr const&);
        /*
                Bool VTrigger(EventDataInterfacePtr const&) const ;
        */

        Bool QueueEvent(EventDataInterfacePtr const&);

#if 0
        Bool VAbortEvent(EventTypeStr const&) override final;
#endif

        Bool Tick(const Uint32);

        Bool ValidateType(EventTypeStr const&) const;
        Bool ValidateHash(const Uint64) const;

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
