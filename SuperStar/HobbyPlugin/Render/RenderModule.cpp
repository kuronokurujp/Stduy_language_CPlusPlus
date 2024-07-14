#include "RenderModule.h"

MODULE_GENRATE_DEFINITION(Render::RenderModule, Render);

namespace Render
{
    /// <summary>
    /// モジュール初期化
    /// </summary>
    /// <returns></returns>
    const Bool RenderModule::Start()
    {
        return TRUE;
    }

    const Bool RenderModule::AddCmd(const Command&& in_rrCmd)
    {
        // コマンドをコピー
        this->_commandBuff.PushBack(in_rrCmd);

        return TRUE;
    }

    /// <summary>
    /// インスタンス破棄時に呼ばれる
    /// </summary>

    const Bool RenderModule::Release()
    {
        return TRUE;
    }

}  // namespace Render
