#include "RenderModule.h"

MODULE_GENRATE_DEFINITION(Render::RenderModule, Render);

namespace Render
{
    /// <summary>
    /// モジュール初期化
    /// </summary>
    /// <returns></returns>
    const Bool RenderModule::Init()
    {
        return TRUE;
    }

    /// <summary>
    /// モジュール終了
    /// </summary>
    /// <returns></returns>
    const Bool RenderModule::End()
    {
        return TRUE;
    }

    const Bool RenderModule::AddCmd(const Command&& in_cmd)
    {
        // コマンドをコピー
        this->_comBuff.PushBack(in_cmd);

        return TRUE;
    }
}
