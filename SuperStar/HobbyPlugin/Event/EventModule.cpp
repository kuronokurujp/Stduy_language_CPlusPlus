#include "EventModule.h"

namespace Event
{
    const Core::Common::Handle EventModule::AddEventManager(
        Core::Memory::UniquePtr<EventManagerStrategyInterface> in_upStrategy)
    {
        Core::Common::Handle handle;

        ++this->_uAddEventMngCount;
        handle.Init(this->_uAddEventMngCount);

        // ストラテジーの所有権を管理インスタンスへ
        this->_mEventMng.Add(handle, HE_NEW(EventManager, 0)(std::move(in_upStrategy)));

        return handle;
    }

    Bool EventModule::RemoveEventManager(const Core::Common::Handle& in_rHandle)
    {
        // TODO: 確保した管理インスタンスを破棄
        if (this->_mEventMng.Contains(in_rHandle) == FALSE) return FALSE;

        auto pEventMng = this->_mEventMng.FindKey(in_rHandle);
        HE_SAFE_DELETE(pEventMng->data);

        return this->_mEventMng.Erase(in_rHandle);
    }

    Bool EventModule::AddListenr(EventListenerPtr const& in_rListener, EventTypeStr const& in_rType)
    {
        for (auto itr = this->_mEventMng.Begin(); itr != this->_mEventMng.End(); ++itr)
        {
            if (itr->data->VAddListenr(in_rListener, in_rType) == FALSE)
            {
                return FALSE;
            }
        }

        return TRUE;
    }

    Bool EventModule::RemoveListener(EventListenerPtr const& in_rListener,
                                     EventTypeStr const& in_rType)
    {
        for (auto itr = this->_mEventMng.Begin(); itr != this->_mEventMng.End(); ++itr)
        {
            itr->data->VRemoveListener(in_rListener, in_rType);
        }

        return TRUE;
    }

    Bool EventModule::QueueEvent(EventDataInterfacePtr const& in_spEventData)
    {
        for (auto itr = this->_mEventMng.Begin(); itr != this->_mEventMng.End(); ++itr)
        {
            if (itr->data->VQueueEvent(in_spEventData) == FALSE)
            {
                return FALSE;
            }
        }

        return TRUE;
    }

    /// <summary>
    /// モジュール初期化
    /// </summary>
    Bool EventModule::_VStart()
    {
        this->_mEventMng.Clear();
        return TRUE;
    }

    /// <summary>
    /// インスタンス破棄時に呼ばれる
    /// </summary>
    Bool EventModule::_VRelease()
    {
        // TODO: 全ての管理インスタンスを破棄
        auto itr = this->_mEventMng.Begin();
        while (itr != this->_mEventMng.End())
        {
            this->RemoveEventManager(itr->key);
            itr = this->_mEventMng.Begin();
        }

        return TRUE;
    }

    Bool EventModule::_VLateUpdate(const Float32 in_fDeltaTime)
    {
        // TODO: 全ての管理インスタンスを実行
        for (auto itr = this->_mEventMng.Begin(); itr != this->_mEventMng.End(); ++itr)
        {
            // TODO: 指定時間でイベント処理を途中終了しないようにする
            // TODO: マルチコアかマルチスレッドを使うかも
            itr->data->VTick(EventManagerInterface::EConstancs_Infinite);
        }

        return TRUE;
    }

}  // namespace Event
