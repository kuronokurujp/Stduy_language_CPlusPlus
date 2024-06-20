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
    class UIInputRouterComponent : public UIInputIComponentInterface
    {
        GENERATED_CLASS_BODY_HEADER();

    public:
        /// <summary>
        /// Processes the input.
        /// </summary>
        virtual void ProcessInput(Platform::InputSystemInterface*) override;

        void AddTerminal(UIInputIComponentInterface*);
        void RemoveTeminal(UIInputIComponentInterface*);

    private:
        Core::Common::FastFixArray<UIInputIComponentInterface*, 512> _terminals;
    };
}  // namespace UI
