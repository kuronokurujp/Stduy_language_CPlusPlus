#include "EventModule.h"

namespace Event
{
    const Core::Common::Handle EventModule::AddEventManager(
        std::unique_ptr<EventManagerStrategyInterface> in_upStrategy)
    {
        Core::Common::Handle handle;

        ++this->_uAddEventMngCount;
        handle.Init(this->_uAddEventMngCount);

        // TODO: ストラテジーの所有権を管理インスタンスへ
        this->_mEventMng.Add(handle, HE_NEW(EventManager, 0)(std::move(in_upStrategy)));

        return handle;
    }

    const Bool EventModule::RemoveEventManager(const Core::Common::Handle& in_rHandle)
    {
        // TODO: 確保した管理インスタンスを破棄
        if (this->_mEventMng.Contains(in_rHandle) == FALSE) return FALSE;

        auto pEventMng = this->_mEventMng.FindKey(in_rHandle);
        HE_SAFE_DELETE(pEventMng->data);

        this->_mEventMng.Erase(in_rHandle);

        return Bool();
    }

    /// <summary>
    /// モジュール初期化
    /// </summary>
    const Bool EventModule::_VStart()
    {
        return TRUE;
    }

    /// <summary>
    /// インスタンス破棄時に呼ばれる
    /// </summary>
    const Bool EventModule::_VRelease()
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

    const Bool EventModule::_VLateUpdate(const Float32 in_fDeltaTime)
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
