#include "ActorModule.h"

MODULE_GENRATE_DEFINITION(Actor::ActorModule, Actor);

namespace Actor
{
    /// <summary>
    /// モジュール初期化
    /// </summary>
    /// <returns></returns>
    const Bool ActorModule::Start()
    {
        return TRUE;
    }

    /// <summary>
    /// インスタンス破棄時に呼ばれる
    /// </summary>
    /// <returns></returns>
    const Bool ActorModule::Release()
    {
        return TRUE;
    }
}  // namespace Actor
