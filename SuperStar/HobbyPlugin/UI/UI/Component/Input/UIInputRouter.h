#pragma once

#include "Actor/Component/InputComponent.h"

namespace UI
{
    /// <summary>
    /// UIの入力ルーター
    /// UIのWidgetのタップやボタン入力を行い,
    /// 各Widgetへ結果を送信する
    /// </summary>
    class UIInputRouterStrategy final : public Actor::InputStrategyBase
    {
        HE_CLASS_COPY_NG(UIInputRouterStrategy);

    public:
        UIInputRouterStrategy() : Actor::InputStrategyBase() {}

        void VProcessInput(const void* in_pInputMap, Actor::Object*) override final;
    };
}  // namespace UI
