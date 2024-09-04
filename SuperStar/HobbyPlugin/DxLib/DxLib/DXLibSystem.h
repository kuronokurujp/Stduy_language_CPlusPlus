#pragma once

#include "Engine/Platform/PlatformSystem.h"

namespace DXLib
{
    /// <summary>
    /// DXLibプラットフォームのシステム
    /// </summary>
    class System final : public Platform::SystemInterface
    {
    public:
        const Uint32 VGetRand(const Uint32) override final;
        const Bool VSetSeedRand(const Uint32) override final;
    };
}  // namespace DXLib
