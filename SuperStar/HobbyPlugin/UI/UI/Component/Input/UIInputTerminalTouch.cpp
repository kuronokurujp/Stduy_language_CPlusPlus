#include "UIInputTerminalTouch.h"

#include "Engine/Common/CustomVector.h"
#include "UI/Component/Widget/UIWidget.h"
#include "UI/Widget.h"

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
        Core::Common::CustomFixStack<Actor::Component*, 128> sWidgetComponent;
        pWidget->OutputChildrenComponent(&sWidgetComponent, &UIWidgetComponent::CLASS_RTTI);

        Uint32 uSize = sWidgetComponent.Size();
        Core::Common::Handle handle;
        while (sWidgetComponent.Empty() == FALSE)
        {
            UIWidgetComponent* c = reinterpret_cast<UIWidgetComponent*>(sWidgetComponent.PopBack());
            c->OnTouch(rState._touch);
        }
    }
}  // namespace UI
