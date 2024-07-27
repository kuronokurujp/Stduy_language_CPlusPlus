#pragma once

#include "Engine/Common/CustomVector.h"
#include "UIInputInterface.h"

namespace UI
{
    /// <summary>
    /// UIの入力ルーター
    /// UIのWidgetのタップやボタン入力を行い,
    /// 各Widgetへ結果を送信する
    /// </summary>
    class UIInputRouterComponent : public Actor::Component
    {
        GENERATED_CLASS_BODY_HEADER(UIInputRouterComponent, Actor::Component);

    public:
        /// <summary>
        /// Processes the input.
        /// </summary>
        void ProcessInput(Platform::InputSystemInterface*,
                          Core::Common::StackBase<Actor::Component*>&);
    };
}  // namespace UI
