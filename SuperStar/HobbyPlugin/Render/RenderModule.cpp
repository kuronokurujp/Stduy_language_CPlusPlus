#include "RenderModule.h"

namespace Render
{
    /// <summary>
    /// モジュール初期化
    /// </summary>
    /// <returns></returns>
    const Bool RenderModule::_Start()
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

    const Bool RenderModule::_Release()
    {
        return TRUE;
    }

}  // namespace Render
