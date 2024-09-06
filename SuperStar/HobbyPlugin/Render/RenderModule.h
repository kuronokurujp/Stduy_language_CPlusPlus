#pragma once

#include "Engine/Common/CustomVector.h"
#include "Engine/Common/PoolManager.h"
#include "Engine/Module/Module.h"

// モジュールのヘッダーファイルは全てインクルードする
#include "Render/Color.h"
#include "Render/Command/Command.h"
#include "Render/View/View.h"

namespace Render
{
    /// <summary>
    /// 描画モジュール
    /// </summary>
    class RenderModule final : public Module::ModuleBase
    {
        HE_CLASS_COPY_NG(RenderModule);
        HE_CLASS_MOVE_NG(RenderModule);
        HE_MODULE_GENRATE_DECLARATION(RenderModule);

    public:
        using ViewHandleVector = Core::Common::CustomFixVector<Core::Common::Handle, 32>;

        enum EPriority
        {
            EPriority_None = 0xffff,
        };

    public:
        RenderModule() : ModuleBase(ModuleName(), Module::ELayer_View, 10) {}

        /// <summary>
        /// レンダリングビューを追加
        /// </summary>
        /// <returns></returns>
        const Core::Common::Handle AddView(const Uint32 in_uPriority = EPriority_None);

        /// <summary>
        /// 追加したレンダリングビューを削除
        /// </summary>
        const Bool RemoveView(const Core::Common::Handle&);

        /// <summary>
        /// レンダリングビュー取得
        /// </summary>
        inline View* GetView(const Core::Common::Handle& in_rHandle)
        {
            if (in_rHandle.Null()) return NULL;

            return this->_poolView.Ref(in_rHandle);
        }

        /// <summary>
        /// ビューのハンドル群を取得
        /// </summary>
        /// <returns></returns>
        inline const ViewHandleVector& ViewHandles() const { return this->_vViewHandle; }

    protected:
        /// <summary>
        /// モジュール初期化
        /// </summary>
        /// <returns></returns>
        const Bool _VStart() override final;
        /// <summary>
        /// インスタンス破棄時に呼ばれる
        /// </summary>
        const Bool _VRelease() override final;

        /// <summary>
        /// 前更新
        /// </summary>
        const Bool _VBeforeUpdate(const Float32 in_fDeltaTime) override final;

    private:
        Core::Common::FixPoolManager<View, 32> _poolView;
        ViewHandleVector _vViewHandle;
    };

}  // namespace Render
