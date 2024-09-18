#include "RenderModule.h"

namespace Render
{
    const Core::Common::Handle RenderModule::AddView(const Uint32 in_uPriority)
    {
        Core::Common::Handle handle;
        if (this->_poolView.Alloc(&handle) == NULL) return NullHandle;

        const Bool bFrontPriority =
            (in_uPriority == EPriority_None) || (this->_vViewHandle.Size() <= in_uPriority);
        if (bFrontPriority)
        {
            this->_vViewHandle.PushBack(handle);
        }
        else
        {
            this->_vViewHandle.Insert(in_uPriority, handle);
        }

        return handle;
    }

    Bool RenderModule::RemoveView(const Core::Common::Handle& in_rHandle)
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
    Bool RenderModule::_VStart()
    {
        return TRUE;
    }

    /// <summary>
    /// インスタンス破棄時に呼ばれる
    /// </summary>
    Bool RenderModule::_VRelease()
    {
        // ビュー解放
        for (Uint32 i = 0; i < this->_vViewHandle.Size(); ++i)
        {
            this->_poolView.Free(this->_vViewHandle[i]);
        }

        return TRUE;
    }

    Bool RenderModule::_VBeforeUpdate(const Float32 in_fDeltaTime)
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
