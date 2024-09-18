#pragma once

#include "Engine/Platform/PlatformTime.h"

namespace DXLib
{
    /// <summary>
    /// プラットフォームの時間インターフェイス
    /// </summary>
    class Time final : public Platform::TimeInterface
    {
    public:
        Uint32 VNowMSec() override final;
        void VSleepMSec(const Uint32 in_uMs) override final;
    };
}  // namespace DXLib
