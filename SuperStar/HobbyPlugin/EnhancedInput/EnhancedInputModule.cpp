#include "EnhancedInputModule.h"

#include "Engine/Common/CustomMap.h"
#include "Engine/Memory/Memory.h"

namespace EnhancedInput
{

    EnhancedInputModule::EnhancedInputModule() : ModuleBase(ModuleName())
    {
    }

    /// <summary>
    /// UIのモジュール初期化
    /// </summary>
    /// <returns></returns>
    const Bool EnhancedInputModule::_Start()
    {
        return TRUE;
    }

    const Bool EnhancedInputModule::_Release()
    {
        return TRUE;
    }
}  // namespace EnhancedInput
