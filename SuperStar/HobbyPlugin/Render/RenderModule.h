#pragma once

#include "Module/Module.h"

// モジュールのヘッダーファイルは全てインクルードする
#include "RenderType.h"
#include "CommandBuffer.h"
#include "Command/Command.h"

namespace Render
{
    /// <summary>
    /// 描画モジュール
    /// </summary>
    class RenderModule final : public Module::ModuleBase<RenderModule>
    {
    public:
        /// <summary>
        /// モジュール初期化
        /// </summary>
        /// <returns></returns>
        const Bool Init() final override;

        /// <summary>
        /// モジュール終了
        /// </summary>
        /// <returns></returns>
        const Bool End() final override;

        // 描画コマンドを追加
        const Bool AddCmd(const Command&& in_cmd);

        /// <summary>
        /// コマンドをクリア
        /// </summary>
        void ClearCmd() { this->_comBuff.Empty(); }

        /// <summary>
        /// コマンド配列取得
        /// </summary>
        /// <returns></returns>
        CommandBuffer* GetCmdBuff() { return &this->_comBuff; }

    private:
        CommandBuffer _comBuff;
    };
}

MODULE_GENRATE_DECLARATION(Render::RenderModule, Render);
