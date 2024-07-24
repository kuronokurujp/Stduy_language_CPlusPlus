#pragma once

#include "Engine/Platform/PlatformModule.h"

namespace DXLib
{
    /// <summary>
    /// プラットフォームの時間インターフェイス
    /// </summary>
    class TimeSystem : public Platform::TimeSystemInterface
    {
    public:
        const Uint32 NowMSec() override final;
        void SleepMSec(const Uint32 in_uMs) override final;
    };
}  // namespace DXLib
