#pragma once

#include "Engine/Core.h"

// プラットフォームのシステム周りのインターフェイス

namespace Platform
{
    class SystemInterface
    {
    public:
        virtual const Uint32 GetRand(const Uint32)   = 0;
        virtual const Bool SetSeedRand(const Uint32) = 0;
    };

}  // namespace Platform
