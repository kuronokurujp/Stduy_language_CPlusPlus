#pragma once

// DXLib用の入力システム
#include "Engine/Platform/PlatformInput.h"

namespace DXLib
{
    /// <summary>
    /// DxLib用の入力システム
    /// </summary>
    class Input final : public Platform::InputInterface
    {
    public:
        void VInit() override final;
        void VUpdate(const Float32 in_fDeltaTime) override final;

    private:
        Uint32 _uCurrButton = 0;
        Uint32 _uPrevButton = 0;
    };
}  // namespace DXLib
