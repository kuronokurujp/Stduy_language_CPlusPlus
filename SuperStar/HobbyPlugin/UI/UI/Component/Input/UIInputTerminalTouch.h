#pragma once

#include "UIInputInterface.h"

namespace UI
{
    /// <summary>
    /// 入力ルーターのタッチ入力端末
    /// </summary>
    class UIInputTouchComponent : public UIInputIComponentInterface
    {
        GENERATED_CLASS_BODY_HEADER(UIInputTouchComponent, UIInputIComponentInterface);

    public:
        /// <summary>
        /// Processes the input.
        /// </summary>
        virtual void ProcessInput(const void* in_pInputMap) override;
    };
}  // namespace UI
