#pragma once

#include "Command/Command.h"
#include "Core/Common/FixArray.h"

namespace Render
{
    /// <summary>
    /// コマンドバッファ
    /// </summary>
    class CommandBuffer : public Core::Common::FastFixArray<Command, 128>
    {
    };
}  // namespace Render
