#include "RenderModule.h"

namespace Render
{
    const Core::Common::Handle RenderModule::AddView()
    {
        Core::Common::Handle handle;
        if (this->_poolView.Alloc(&handle) == NULL) return InvalidHandle;

        this->_vViewHandle.PushBack(handle);

        return handle;
    }

    const Bool RenderModule::RemoveView(const Core::Common::Handle& in_rHandle)
    {
        if (in_rHandle.Null()) return FALSE;

        // Viewの解放処理をする
        auto pView = this->_poolView.Ref(in_rHandle);
        pView->Release();

        // 解放が終わったらプールしているデータを解放
        this->_poolView.Free(in_rHandle);

        // 管理リストから外す
        return this->_vViewHandle.Remove(in_rHandle);
    }

    /// <summary>
    /// モジュール初期化
    /// </summary>
    /// <returns></returns>
    const Bool RenderModule::_Start()
    {
        return TRUE;
    }

    /// <summary>
    /// インスタンス破棄時に呼ばれる
    /// </summary>

    const Bool RenderModule::_Release()
    {
        // ビュー解放
        for (Uint32 i = 0; i < this->_vViewHandle.Size(); ++i)
        {
            this->_poolView.Free(this->_vViewHandle[i]);
        }

        return TRUE;
    }

    const Bool RenderModule::_BeforeUpdate(const Float32 in_fDeltaTime)
    {
        // ビューのコマンドクリア
        for (Uint32 i = 0; i < this->_vViewHandle.Size(); ++i)
        {
            auto pView = this->_poolView.Ref(this->_vViewHandle[i]);
            if (pView == NULL) continue;

            pView->ClearCmd();
        }

        return TRUE;
    }

}  // namespace Render
