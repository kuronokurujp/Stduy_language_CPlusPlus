#include "UIInputTerminalTouch.h"

#include "../../Widget.h"
#include "../Widget/UIWidget.h"
#include "Core/Common/FixArray.h"

namespace UI
{

    void UIInputTouchComponent::ProcessInput(Platform::InputSystemInterface* in_pInput)
    {
        HE_ASSERT(in_pInput);

        // タッチ入力を検知
        const Platform::InputState& rState = in_pInput->GetState();

        // 入力結果をWidgetに通知
        Widget* pWidget = reinterpret_cast<Widget*>(this->_pOwner);
        HE_ASSERT(pWidget != NULL);

        // Widgetアクターに設定しているUIWidgetコンポーネントを全て取得
        Core::Common::FastFixArray<Actor::Component*, 128> aWidgetComponent;
        pWidget->OutputChildrenComponent(&aWidgetComponent, &UIWidgetComponent::CLASS_RTTI);

        Uint32 uSize = aWidgetComponent.Size();
        Core::Common::Handle handle;
        for (Uint32 i = 0; i < uSize; ++i)
        {
            UIWidgetComponent* c = reinterpret_cast<UIWidgetComponent*>(aWidgetComponent[i]);
            c->OnTouch(rState._touch);
        }
    }
}  // namespace UI
