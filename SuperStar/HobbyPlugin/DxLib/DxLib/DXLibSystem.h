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
        Uint32 VGetRand(const Uint32) override final;
        Bool VSetSeedRand(const Uint32) override final;
    };
}  // namespace DXLib
