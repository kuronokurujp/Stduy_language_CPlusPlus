#pragma once

#include "Engine/Platform/PlatformSystem.h"

namespace DXLib
{
    /// <summary>
    /// DXLibプラットフォームのシステム
    /// </summary>
    class System : public Platform::SystemInterface
    {
    public:
        const Uint32 GetRand(const Uint32) override final;
        const Bool SetSeedRand(const Uint32) override final;
    };
}  // namespace DXLib
