#include "UIInputRouter.h"

#include "ActorModule.h"
#include "EnhancedInputModule.h"
#include "UI/Component/Widget/UIWidget.h"
#include "UI/Widget.h"

namespace UI
{
    void UIInputRouterStrategy::VProcessInput(const void* in_pInputMap,
                                              Actor::Object* in_pSelfObject)
    {
        HE_ASSERT(in_pInputMap);
        HE_ASSERT(in_pSelfObject);

        auto pInputMap = reinterpret_cast<const EnhancedInput::InputMap*>(in_pInputMap);
        HE_ASSERT(pInputMap);

        // UIのユーザー入力があるかチェック
        if (pInputMap->Contains(HE_STR_TEXT("UIButton")) == FALSE) return;

        auto input = pInputMap->FindKey(HE_STR_TEXT("UIButton"))->data;

        // 入力結果をWidgetに通知
        Widget* pWidget = reinterpret_cast<Widget*>(in_pSelfObject);
        HE_ASSERT(pWidget != NULL);

        // Widgetアクターに設定しているUIWidgetコンポーネントを全て取得
        Core::Common::CustomFixStack<Actor::Component*, 128> sWidgetComponent;
        pWidget->OutputChildrenComponent(&sWidgetComponent, &UIWidgetComponent::CLASS_RTTI);

        Uint32 uSize = sWidgetComponent.Size();
        Core::Common::Handle handle;

        // UIに関わる, マウスのクリックやキーボードなどの結果を受け取り, 各入力端末へ通知する
        // ルーター側でUIの入力処理を制御
        // UIレイヤーに応じてボタンを押せないとか
        for (Uint32 i = 0; i < input.Size(); ++i)
        {
            if (input[i].eType == EnhancedInput::EInputType::EInputType_Touch)
            {
                while (sWidgetComponent.Empty() == FALSE)
                {
                    UIWidgetComponent* c =
                        reinterpret_cast<UIWidgetComponent*>(sWidgetComponent.PopBack());
                    c->VOnTouch(input[i].item.touch);
                }
            }
        }
    }

}  // namespace UI
