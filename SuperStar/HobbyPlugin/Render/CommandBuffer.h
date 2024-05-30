#pragma once

#include "Core/Common/FixArray.h"
#include "Command/Command.h"

namespace Render
{
    /// <summary>
    /// コマンドバッファ
    /// </summary>
    class CommandBuffer : public Core::Common::FastFixArray<Command, 128>
    {
    public:

    };
}
