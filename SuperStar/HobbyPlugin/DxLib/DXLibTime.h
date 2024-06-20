#pragma once

#include "Platform/PlatformModule.h"

namespace DXLib
{
    /// <summary>
    /// プラットフォームの時間インターフェイス
    /// </summary>
    class TimeSystem : public Platform::TimeSystemInterface
    {
    public:
        const Uint32 NowMSec() override final;
        void SleepMSec(const Uint32 in_ms) override final;
    };
}  // namespace DXLib
