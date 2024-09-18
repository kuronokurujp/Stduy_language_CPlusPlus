#include "ActorModule.h"

namespace Actor
{
    /// <summary>
    /// モジュール初期化
    /// </summary>
    /// <returns></returns>
    Bool ActorModule::_VStart()
    {
        return TRUE;
    }

    /// <summary>
    /// インスタンス破棄時に呼ばれる
    /// </summary>
    /// <returns></returns>
    Bool ActorModule::_VRelease()
    {
        return TRUE;
    }
}  // namespace Actor
