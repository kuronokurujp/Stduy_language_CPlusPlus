#include "UIInputTerminalTouch.h"

#include "Engine/Common/CustomVector.h"
#include "EnhancedInputModule.h"
#include "UI/Component/Widget/UIWidget.h"
#include "UI/Widget.h"

namespace UI
{
    void UIInputTouchComponent::ProcessInput(const void* in_pInputMap)
    {
        HE_ASSERT(in_pInputMap);
        auto pInputMap = reinterpret_cast<const EnhancedInput::InputMap*>(in_pInputMap);
        HE_ASSERT(pInputMap);

        if (pInputMap->Contains(HE_STR_TEXT("UIButton")) == FALSE) return;

        auto input = pInputMap->FindKey(HE_STR_TEXT("UIButton"))->data;

        // 入力結果をWidgetに通知
        Widget* pWidget = reinterpret_cast<Widget*>(this->_pOwner);
        HE_ASSERT(pWidget != NULL);

        // Widgetアクターに設定しているUIWidgetコンポーネントを全て取得
        Core::Common::CustomFixStack<Actor::Component*, 128> sWidgetComponent;
        pWidget->OutputChildrenComponent(&sWidgetComponent, &UIWidgetComponent::CLASS_RTTI);

        Uint32 uSize = sWidgetComponent.Size();
        Core::Common::Handle handle;

        for (Uint32 i = 0; i < input.Size(); ++i)
        {
            if (input[i].eType == EnhancedInput::eInputType::eInputType_Touch)
            {
                while (sWidgetComponent.Empty() == FALSE)
                {
                    UIWidgetComponent* c =
                        reinterpret_cast<UIWidgetComponent*>(sWidgetComponent.PopBack());
                    c->OnTouch(input[i].item.touch);
                }
            }
        }
    }

}  // namespace UI
