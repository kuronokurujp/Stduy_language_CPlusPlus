#pragma once

#include "UIInputInterface.h"

namespace UI
{
    /// <summary>
    /// 入力ルーターのタッチ入力端末
    /// </summary>
    class UIInputTouchComponent : public UIInputIComponentInterface
    {
        GENERATED_CLASS_BODY_HEADER()
    public:
        /// <summary>
        /// Processes the input.
        /// </summary>
        virtual void ProcessInput(Platform::InputSystemInterface*) override;
    };
}
