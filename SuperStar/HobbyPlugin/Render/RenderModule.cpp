#include "RenderModule.h"

namespace Render
{
    /// <summary>
    /// TODO: モジュール初期化
    /// </summary>
    /// <returns></returns>
    const Bool RenderModule::Init()
    {
        return TRUE;
    }

    /// <summary>
    /// TODO: モジュール終了
    /// </summary>
    /// <returns></returns>
    const Bool RenderModule::End()
    {
        return TRUE;
    }

    // TODO: 描画コマンド命令を用意
    const Bool RenderModule::AddCmd(const Command&& in_cmd)
    {
        // TODO: コマンドをコピー
        this->_comBuff.PushBack(in_cmd);

        return TRUE;
    }
}
