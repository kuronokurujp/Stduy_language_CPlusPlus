#pragma once

#include "Engine/Common/CustomArray.h"
#include "Render/Command/Command.h"

namespace Render
{
    /// <summary>
    /// コマンドバッファ
    /// </summary>
    class CommandBuffer : public Core::Common::FastFixArray<Command, 128>
    {
    };
}  // namespace Render
