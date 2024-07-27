#pragma once

#include "Engine/Common/CustomVector.h"
#include "Render/Command/Command.h"

namespace Render
{
    /// <summary>
    /// コマンドバッファ
    /// </summary>
    class CommandBuffer : public Core::Common::CustomFixVector<Command, 128>
    {
    };
}  // namespace Render
