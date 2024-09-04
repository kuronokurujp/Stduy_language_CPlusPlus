#pragma once

#include "Engine/Core.h"
#include "Render/Command/CommandBuffer.h"

namespace Render
{
    /// <summary>
    /// レンダリングするビュー
    /// 描画コマンドをためてコマンドを実行する
    /// </summary>
    class View final
    {
    public:
        const Bool Release();

        // 描画コマンドを追加
        // コマンドデータは呼び出し元のものにするためにstd::move()で渡す
        const Bool AddCmd(const Command&& in_rrCmd);

        /// <summary>
        /// コマンドをクリア
        /// </summary>
        inline void ClearCmd() { this->_commandBuff.Clear(); }

        /// <summary>
        /// コマンドバッファを取得
        /// </summary>
        inline const CommandBuffer* GetCmdBuff() const { return &this->_commandBuff; }

    private:
        CommandBuffer _commandBuff;
    };
}  // namespace Render
