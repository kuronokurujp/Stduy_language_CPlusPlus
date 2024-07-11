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

    const Bool RenderModule::AddCmd(const Command&& in_cmd)
    {
        // コマンドをコピー
        this->_comBuff.PushBack(in_cmd);

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
