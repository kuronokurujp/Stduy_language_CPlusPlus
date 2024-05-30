#pragma once

// TODO: DXLib用の入力システム

#include "DxLib.h"

#include "Platform/PlatformInput.h"

namespace DXLib
{
    /// <summary>
    /// TODO: DxLib用の入力システム
    /// </summary>
    class InputSystem : public Platform::InputSystemInterface
    {
    public:
        void Init() final override;
        void BeforeUpdate(const Float32 in_deltaTime) final override;
        void Update(const Float32 in_deltaTime) final override;
        void AfterUpdate(const Float32 in_deltaTime) final override;

    private:
        Uint32 _currButton = 0;
        Uint32 _prevButton = 0;
    };
}
