#pragma once

#include "Command/Command.h"
#include "Common/CustomArray.h"

namespace Render
{
    /// <summary>
    /// コマンドバッファ
    /// </summary>
    class CommandBuffer : public Core::Common::FastFixArray<Command, 128>
    {
    };
}  // namespace Render
