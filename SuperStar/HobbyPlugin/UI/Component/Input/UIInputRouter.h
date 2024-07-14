#pragma once

#include "HobbyEngine/Core/Common/FixArray.h"
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
                          Core::Common::FastFixArrayBase<Actor::Component*>&);
    };
}  // namespace UI
