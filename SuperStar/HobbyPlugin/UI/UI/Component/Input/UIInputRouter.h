#pragma once

#include "Actor/Component/InputComponent.h"

namespace UI
{
    /// <summary>
    /// UIの入力ルーター
    /// UIのWidgetのタップやボタン入力を行い,
    /// 各Widgetへ結果を送信する
    /// </summary>
    class UIInputRouterComponent final : public Actor::InputComponent
    {
        HE_CLASS_COPY_NG(UIInputRouterComponent);
        GENERATED_CLASS_BODY_HEADER(UIInputRouterComponent, Actor::InputComponent);

    public:
        UIInputRouterComponent() : Actor::InputComponent() {}

        /// <summary>
        /// Processes the input.
        /// </summary>
        void VProcessInput(const void* in_pInputMap) override final;
    };
}  // namespace UI
