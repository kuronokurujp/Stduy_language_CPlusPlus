#pragma once

// DXLib用の入力システム

#include "Platform/PlatformInput.h"

namespace DXLib
{
    /// <summary>
    /// DxLib用の入力システム
    /// </summary>
    class InputSystem : public Platform::InputSystemInterface
    {
    public:
        void Init() final override;
        void BeforeUpdate(const Float32 in_fDeltaTime) final override;
        void Update(const Float32 in_fDeltaTime) final override;
        void AfterUpdate(const Float32 in_fDeltaTime) final override;

    private:
        Uint32 _uCurrButton = 0;
        Uint32 _uPrevButton = 0;
    };
}  // namespace DXLib
