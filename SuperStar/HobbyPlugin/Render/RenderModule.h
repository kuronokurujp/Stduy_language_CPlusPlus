#pragma once

#include "Engine/Module/Module.h"

// モジュールのヘッダーファイルは全てインクルードする
#include "Render/Color.h"
#include "Render/Command/Command.h"
#include "Render/CommandBuffer.h"

namespace Render
{
    /// <summary>
    /// 描画モジュール
    /// </summary>
    class RenderModule final : public Module::ModuleBase
    {
    public:
        RenderModule(const Char* in_szName) : ModuleBase(in_szName) {}

        // 描画コマンドを追加
        // コマンドデータは呼び出し元のものにするためにstd::move()で渡す
        const Bool AddCmd(const Command&& in_rrCmd);

        /// <summary>
        /// コマンドをクリア
        /// </summary>
        void ClearCmd() { this->_commandBuff.Clear(); }

        /// <summary>
        /// コマンド配列取得
        /// </summary>
        /// <returns></returns>
        CommandBuffer* GetCmdBuff() { return &this->_commandBuff; }

    protected:
        /// <summary>
        /// モジュール初期化
        /// </summary>
        /// <returns></returns>
        const Bool Start() final override;
        /// <summary>
        /// インスタンス破棄時に呼ばれる
        /// </summary>
        virtual const Bool Release() override final;

    private:
        CommandBuffer _commandBuff;
    };
}  // namespace Render

MODULE_GENRATE_DECLARATION(Render::RenderModule, Render);
