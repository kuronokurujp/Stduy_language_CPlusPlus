#pragma once

#include "Engine/Core.h"

// プラットフォームのシステム周りのインターフェイス

namespace Platform
{
    class SystemInterface
    {
    public:
        virtual ~SystemInterface()              = default;
        virtual Uint32 VGetRand(const Uint32)   = 0;
        virtual Bool VSetSeedRand(const Uint32) = 0;
    };

}  // namespace Platform
