﻿#include "EventManager.h"

#include <chrono>

namespace Event
{
    EventManager::EventManager(std::unique_ptr<EventManagerStrategyInterface> in_upStrategy)
        : _sActiveQueue(0)
    {
        this->_upStrategy = std::move(in_upStrategy);
    }

    EventManager::~EventManager()
    {
        this->_sActiveQueue = 0;
    }

    const Bool EventManager::EmptyEvent() const
    {
        for (Uint32 i = 0; i < HE_ARRAY_NUM(this->_aQueue); ++i)
        {
            if (0 < this->_aQueue[i].size()) return FALSE;
        }

        return TRUE;
    }

    const Bool EventManager::VAddListenr(EventListenerPtr const& in_rListener,
                                         EventTypeStr const& in_rType)
    {
        if (this->VValidateType(in_rType) == FALSE) return FALSE;

        // リスナーマップのエントリ探し、
        // エントリに対応するテーブルがなければエントリ作成
        auto itr = this->_mRegistry.FindKey(in_rType.Hash());

        // リスナーに登録されていない場合は登録する
        if (itr == this->_mRegistry.End())
        {
            itr = this->_mRegistry.Add(in_rType.Hash(), EventListenerTable());
            if (itr.IsValid() == FALSE) return FALSE;
        }

        // リスナーマップリスト更新

        // mapテーブルからハッシュ指定したテーブルデータリストを取得
        EventListenerTable& rTable = itr->data;

        // マップから取得した要素にあるリスナーリストにすでに登録されているかチェック
        // すでに登録済みならリスト登録はスキップ
        for (EventListenerTable::iterator it = rTable.begin(), itEnd = rTable.end(); it != itEnd;
             it++)
        {
            if (*it == in_rListener) return FALSE;
        }

        // イベント型の有効性を確認
        // イベントリスナーの有効性を確認
        // イベントリスナーはまだマップにないから追加
        rTable.push_back(in_rListener);

        // 登録に成功
        return TRUE;
    }

    const Bool EventManager::VRemoveListener(EventListenerPtr const& in_rListener,
                                             EventTypeStr const& in_rType)
    {
        if (this->VValidateType(in_rType) == FALSE) return FALSE;
        Bool bErase = FALSE;

        // 総当りの手法 合致するリスナーが見つかるまで、
        // 既存のマッピングのエントリをたどる、見つかったら削除する
        for (auto itr = this->_mRegistry.Begin(); itr != this->_mRegistry.End(); ++itr)
        {
            EventListenerTable& table = itr->data;
            for (EventListenerTable::iterator it2 = table.begin(), it2End = table.end();
                 it2 != it2End; it2++)
            {
                if (*it2 == in_rListener)
                {
                    // 目的のリスナーを発見。テーブルから削除
                    table.erase(it2);

                    // リターンコードを更新
                    bErase = TRUE;

                    // VAddListenr()でリスナーの重複がないようにしているから
                    // これ以上の検索は不要

                    break;
                }
            }
        }

        // TODO: リスナーが一つもないならリスナー枠を削除

        return bErase;
    }

    const Bool EventManager::VTrigger(EventDataInterfacePtr const& in_rEvent) const
    {
        Bool bProc = FALSE;

        auto itrSpecalEvant = this->_mRegistry.FindKey(BaseEventData::EType_SpecalEvent);
        if (itrSpecalEvant != this->_mRegistry.End())
        {
            EventListenerTable const& table = itrSpecalEvant->data;
            for (EventListenerTable::const_iterator it2 = table.begin(), it2End = table.end();
                 it2 != it2End; it2++)
            {
                if ((*it2)->VHandleEvent(in_rEvent))
                {
                    bProc = TRUE;
                }
            }
        }

        if (this->VValidateType(in_rEvent->VEventTypeStr()) == FALSE) return bProc;

        auto itr = this->_mRegistry.FindKey(in_rEvent->VEventTypeStr().Hash());
        if (itr == this->_mRegistry.End()) return bProc;

        EventListenerTable const& table = itr->data;
        for (EventListenerTable::const_iterator it2 = table.begin(), it2End = table.end();
             it2 != it2End; it2++)
        {
            EventListenerPtr listener = *it2;
            if (listener->VHandleEvent(in_rEvent))
            {
                // メッセージを受け入れる場合は trueにする
                bProc = TRUE;
            }
        }

        // イベント処理が一つでも成功したか
        return bProc;
    }

    const Bool EventManager::VQueueEvent(EventDataInterfacePtr const& in_rEvent)
    {
        HE_ASSERT(0 <= this->_sActiveQueue);
        HE_ASSERT(this->_sActiveQueue < EConstants_NumQueues);

        if (this->VValidateType(in_rEvent->VEventTypeStr()) == FALSE) return FALSE;

        this->_aQueue[this->_sActiveQueue].push_back(in_rEvent);

        return TRUE;
    }
#if 0
    const Bool EventManager::VAbortEvent(EventTypeStr const& in_rType)
    {
        HE_ASSERT(0 <= this->_sActiveQueue);
        HE_ASSERT(this->_sActiveQueue < EConstants_NumQueues);

        if (this->VValidateType(in_rType) == FALSE) return FALSE;

        auto it = this->_mRegistry.FindKey(in_rType.Hash());

        // イベントタイプのハッシュに対応したリスナーがあるかどうか
        if (it == this->_mRegistry.End()) return FALSE;

        Bool bProcResult     = FALSE;
        EventQueue& evtQueue = this->_aQueue[this->_sActiveQueue];

        for (EventQueue::iterator it = evtQueue.begin(), itEnd = evtQueue.end(); it != itEnd; it++)
        {
            if ((*it)->VEventTypeStr() == in_rType)
            {
                it          = evtQueue.erase(it);
                bProcResult = TRUE;
            }
            else
            {
                ++it;
            }
        }

        return bProcResult;
    }
#endif

    const Bool EventManager::VTick(const Uint32 in_uMaxMillis)
    {
        // 計測開始時間
        auto startClock = std::chrono::system_clock::now();

        // アクティブなキューを交換する。
        // 交換後の新しいキューは空でなければならない。
        Sint32 sQueueToProcess = this->_sActiveQueue;

        // 新しいキューのイベントはクリア
        this->_sActiveQueue = (this->_sActiveQueue + 1) % EConstants_NumQueues;
        this->_aQueue[this->_sActiveQueue].clear();

        // 特殊イベントをリスナーが受け取り処理
        auto itSpecalEventListener = this->_mRegistry.FindKey(BaseEventData::EType_SpecalEvent);

        while (0 < this->_aQueue[sQueueToProcess].size())
        {
            // リスナーに投げるイベント取り出す
            EventDataInterfacePtr spEvent = this->_aQueue[sQueueToProcess].front();
            this->_aQueue[sQueueToProcess].pop_front();

            // 特殊イベントタイプのリスナー処理
            {
                if (itSpecalEventListener != this->_mRegistry.End())
                {
                    EventListenerTable const& table = itSpecalEventListener->data;
                    for (EventListenerTable::const_iterator it2    = table.begin(),
                                                            it2End = table.end();
                         it2 != it2End; it2++)
                    {
                        // 特殊イベントは処理失敗しても継続
                        (*it2)->VHandleEvent(spEvent);
                    }
                }
            }

            // イベントタイプに対応したリスナーにイベントを投げる
            {
                // イベントタイプに対応したリスナーを取得
                EventTypeStr const& rEventType = spEvent->VEventTypeStr();
                auto itListeners               = this->_mRegistry.FindKey(rEventType.Hash());

                // イベントで指定したタイプのリスナーがいないのでスキップ
                if (itListeners == this->_mRegistry.End()) continue;

                // イベントタイプに対応したリスナーを呼ぶ
                const Uint32 kEventId           = itListeners->key;
                EventListenerTable const& table = itListeners->data;

                for (EventListenerTable::const_iterator it = table.begin(), end = table.end();
                     it != end; it++)
                {
                    // リスナーがイベント処理に失敗したら以降のリスナーはイベント処理しない
                    if ((*it)->VHandleEvent(spEvent)) break;
                }

                // 指定時間を超えた場合はイベント処理を中断
                if (in_uMaxMillis != EventManagerInterface::EConstancs_Infinite)
                {
                    auto endClock = std::chrono::system_clock::now();

                    // 指定時間を超えたらイベント処理を中断
                    // 処理に要した時間をミリ秒に変換
                    const Uint32 fElapsed =
                        std::chrono::duration_cast<std::chrono::milliseconds>(endClock - startClock)
                            .count();
                    if (in_uMaxMillis <= fElapsed) break;
                }
            }
        }

        const Bool bQueueFlushed = (this->_aQueue[sQueueToProcess].size() == 0);
        {
            // キュー内にイベントが残っている場合は次のフレームでイベントを処理するようにアクティブリストに入れる
            while (0 < this->_aQueue[sQueueToProcess].size())
            {
                EventDataInterfacePtr event = this->_aQueue[sQueueToProcess].back();
                this->_aQueue[sQueueToProcess].pop_back();
                this->_aQueue[this->_sActiveQueue].push_front(event);
            }
        }

        // キュー内にイベントがないか
        return bQueueFlushed;
    }

    const Bool EventManager::VValidateType(EventTypeStr const& in_rType) const
    {
        if (in_rType.Length() <= 0) return FALSE;

        if ((in_rType.Hash() == 0) && (HE_STR_CMP(in_rType.Str(), HE_STR_TEXT("*")) != 0))
            return FALSE;

        if (this->_upStrategy->VIsEventTypeStr(in_rType) == FALSE) return FALSE;

        return TRUE;
    }

    // 情報探索メソッド

    // 特定のイベント型に関連づけられたリスナーのリストを取得
    const Bool EventManager::OutputListenerList(EventListenerList* out,
                                                EventTypeStr const& in_rEventType) const
    {
        HE_ASSERT(out);

        // 無効なイベント型
        if (this->VValidateType(in_rEventType) == FALSE) return FALSE;

        auto itListeners = this->_mRegistry.FindKey(in_rEventType.Hash());

        // イベント型には現在リスナーがない
        if (itListeners == this->_mRegistry.End()) return FALSE;

        EventListenerTable const& table = itListeners->data;

        // このイベント型には以前はリスナーがあったが今はない
        if (table.size() <= 0) return FALSE;

        out->reserve(table.size());
        out->clear();

        for (EventListenerTable::const_iterator it = table.begin(), end = table.end(); it != end;
             it++)
        {
            out->push_back(*it);
        }

        return TRUE;
    }

}  // namespace Event