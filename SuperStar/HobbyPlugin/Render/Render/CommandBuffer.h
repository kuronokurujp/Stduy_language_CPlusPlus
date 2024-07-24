#pragma once

#include "Render/Command/Command.h"
#include "Engine/Common/CustomArray.h"

namespace Render
{
    /// <summary>
    /// コマンドバッファ
    /// </summary>
    class CommandBuffer : public Core::Common::FastFixArray<Command, 128>
    {
    };
}  // namespace Render
