#pragma once

#include "Module/Module.h"
#include "CommandBuffer.h"

namespace Render
{
    /// <summary>
    /// 描画の追加モジュール
    /// </summary>
    class RenderModule final : public Module::ModuleBase<RenderModule>
    {
    public:
        /// <summary>
        /// TODO: モジュール初期化
        /// </summary>
        /// <returns></returns>
        const Bool Init() final override;

        /// <summary>
        /// TODO: モジュール終了
        /// </summary>
        /// <returns></returns>
        const Bool End() final override;

        // TODO: 描画コマンド命令を用意
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
