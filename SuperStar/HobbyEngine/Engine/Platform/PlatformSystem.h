#pragma once

#include "Engine/Core.h"

// プラットフォームのシステム周りのインターフェイス

namespace Platform
{
    class SystemInterface
    {
    public:
        virtual ~SystemInterface()                    = default;
        virtual const Uint32 VGetRand(const Uint32)   = 0;
        virtual const Bool VSetSeedRand(const Uint32) = 0;
    };

}  // namespace Platform
