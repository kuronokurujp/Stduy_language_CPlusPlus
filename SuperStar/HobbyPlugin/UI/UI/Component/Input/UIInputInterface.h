#pragma once

#include "ActorModule.h"
#include "Engine/Platform/PlatformInput.h"

namespace UI
{
    /// <summary>
    /// 入力コンポーネントのインターフェイス
    /// </summary>
    class UIInputIComponentInterface : public Actor::InputComponent
    {
        GENERATED_CLASS_BODY_HEADER(UIInputIComponentInterface, Actor::InputComponent);

    public:
        /// <summary>
        /// Processes the input.
        /// </summary>
        virtual void ProcessInput(const void* in_pInputMap) override { HE_ASSERT(FALSE); }
    };
}  // namespace UI
