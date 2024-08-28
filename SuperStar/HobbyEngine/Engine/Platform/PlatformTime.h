#pragma once

#include "Engine/Core.h"

// プラットフォームの時間周りのインターフェイス

namespace Platform
{
    class TimeInterface
    {
    public:
        virtual const Uint32 VNowMSec()             = 0;
        virtual void VSleepMSec(const Uint32 in_ms) = 0;
    };

}  // namespace Platform
