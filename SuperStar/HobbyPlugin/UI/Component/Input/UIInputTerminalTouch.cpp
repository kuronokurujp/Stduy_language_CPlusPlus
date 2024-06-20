#include "UIInputTerminalTouch.h"

#include "../../Widget.h"
#include "../Widget/UIWidget.h"
#include "Core/Common/FixArray.h"

namespace UI
{
    GENERATED_CLASS_BODY(UIInputTouchComponent, UIInputIComponentInterface);

    void UIInputTouchComponent::ProcessInput(Platform::InputSystemInterface* in_pInput)
    {
        // TODO: オブジェクトの入力処理をする
        E_ASSERT(in_pInput);

        // TODO: タッチ入力を検知
        const Platform::InputState& state = in_pInput->GetState();

        // TODO: 入力結果をWidgetに通知
        Widget* pWidget = reinterpret_cast<Widget*>(this->_pOwner);
        E_ASSERT(pWidget != NULL);

        // TODO: Widgetアクターに設定しているUIWidgetコンポーネントを全て取得
        Core::Common::FastFixArray<Actor::Component*, 128> widgetComponents;
        pWidget->OutputChildComponents(widgetComponents, UIWidgetComponent::CLASS_RTTI);

        for (Uint32 i = 0; i < widgetComponents.Size(); ++i)
        {
            UIWidgetComponent* c = reinterpret_cast<UIWidgetComponent*>(widgetComponents[i]);
            c->OnTouch(state._touch);
        }
    }
}  // namespace UI
