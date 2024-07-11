#pragma once

#include "Module/Module.h"

// モジュールのヘッダーファイルは全てインクルードする
#include "Color.h"
#include "Command/Command.h"
#include "CommandBuffer.h"

namespace Render
{
    /// <summary>
    /// 描画モジュール
    /// </summary>
    class RenderModule final : public Module::ModuleBase
    {
    public:
        RenderModule(const Char* in_pName) : ModuleBase(in_pName) {}

        // 描画コマンドを追加
        const Bool AddCmd(const Command&& in_cmd);

        /// <summary>
        /// コマンドをクリア
        /// </summary>
        void ClearCmd() { this->_comBuff.Clear(); }

        /// <summary>
        /// コマンド配列取得
        /// </summary>
        /// <returns></returns>
        CommandBuffer* GetCmdBuff() { return &this->_comBuff; }

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
        CommandBuffer _comBuff;
    };
}  // namespace Render

MODULE_GENRATE_DECLARATION(Render::RenderModule, Render);
