#pragma once

#include "ActorModule.h"
#include "Platform/PlatformInput.h"

namespace UI
{
    /// <summary>
    /// 入力コンポーネントのインターフェイス
    /// </summary>
    class UIInputIComponentInterface : public Actor::Component
    {
        GENERATED_CLASS_BODY_HEADER(UIInputIComponentInterface, Actor::Component);

    public:
        /// <summary>
        /// Processes the input.
        /// </summary>
        virtual void ProcessInput(Platform::InputSystemInterface*) = 0;
    };
}  // namespace UI
