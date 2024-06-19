#include "ActorModule.h"

#include "Core/Memory/Memory.h"

MODULE_GENRATE_DEFINITION(Actor::ActorModule, Actor);

namespace Actor
{
    /// <summary>
    /// モジュール初期化
    /// </summary>
    /// <returns></returns>
    const Bool ActorModule::Init()
    {
        return TRUE;
    }

    /// <summary>
    /// モジュール終了
    /// </summary>
    /// <returns></returns>
    const Bool ActorModule::End()
    {
        return TRUE;
    }
}
