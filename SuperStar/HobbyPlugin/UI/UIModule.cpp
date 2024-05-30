#include "UIModule.h"

#include "Widget.h"

#include "Component/Widget/UILayer.h"
#include "Component/Widget/UIButton.h"
#include "Component/Widget/UIText.h"

#include "Component/Input/UIInputRouter.h"
#include "Component/Input/UIInputTerminalTouch.h"

#include "Core/Memory/Memory.h"

namespace UI
{
    /// <summary>
    /// TODO: UIのモジュール初期化
    /// </summary>
    /// <returns></returns>
    const Bool UIModule::Init()
    {
        return TRUE;
    }

    /// <summary>
    /// TODO: UIのモジュール終了
    /// </summary>
    /// <returns></returns>
    const Bool UIModule::End()
    {
        return TRUE;
    }

    Core::Common::Handle UIModule::NewLayer(
        const Core::Common::FixString128 in_name,
        const Uint32 in_sort)
    {
        Core::Common::Handle handle = this->NewWidget(in_name, in_sort);

        // 入力ルーターコンポーネントを設定
        auto hInputRouter = this->AddComponent<UI::UIInputRouterComponent>(handle, 0);

        // レイヤーコンポーネントを追加
        auto hLayer = this->AddComponent<UI::UILayerComponent>(handle, 0);

        // TODO: 入力ルーターコンポーネントを設定
        Widget* pWidget = NULL;

#ifdef _HOBBY_ENGINE
        pWidget = reinterpret_cast<Widget*>(HOBBY_CURRENT_LEVEL.GetActor(handle));
        E_ASSERT(pWidget);
#endif

        pWidget->SetInputRouter(hInputRouter);

        return handle;
    }

    Core::Common::Handle UIModule::NewButtonAndText(
        const Core::Common::FixString128 in_name,
        const Uint32 in_sort,
        const Char* in_pText,
        const Core::Math::Rect2& in_rBtnRect, const Uint32 in_btnColor,
        const Core::Math::Vector2& in_textPos, const Uint32 in_textColor)
    {
        Core::Common::Handle handle = this->NewWidget(in_name, in_sort);
        E_ASSERT(handle.Null() == FALSE);

        Actor::Object* pWidget = NULL;
#ifdef _HOBBY_ENGINE
        pWidget = HOBBY_CURRENT_LEVEL.GetActor(handle);
#endif
        if (pWidget == NULL)
            return handle;

        // TODO: ボタンコンポーネント追加と設定
        auto hButton = this->AddComponent<UI::UIButtonComponent>(handle, in_sort);
        {
            UI::UIButtonComponent* pButton = pWidget->GetComponent<UI::UIButtonComponent>(hButton);
            pButton->SetRect(in_rBtnRect);
            pButton->SetColor(in_btnColor);
        }

        // TODO: ボタンの上に表示するテキストコンポーネント追加と設定
        auto hText = this->AddComponent<UI::UITextComponent>(handle, in_sort + 1);
        {
            UI::UITextComponent* pText = pWidget->GetComponent<UI::UITextComponent>(hText);
            pText->SetText(in_pText);
            pText->SetPos(in_textPos);
            pText->SetColor(in_textColor);
        }

        // 入力ルーターの端末コンポーネントを設定
        auto hInputTerminal = this->AddComponent<UI::UIInputTouchComponent>(handle, 0);

        return handle;
    }

    Core::Common::Handle UIModule::NewWidget(const Core::Common::FixString128 in_name, const Uint32 in_sort)
    {
        Core::Common::Handle handle;

        Actor::Object* pWidget = NULL;
#ifdef _HOBBY_ENGINE
        handle = HOBBY_CURRENT_LEVEL.AddActor<Widget>();
        E_ASSERT(handle.Null() == FALSE);

        pWidget = HOBBY_CURRENT_LEVEL.GetActor(handle);
#endif
        if (pWidget == NULL)
            return E_EMPTY_HANDLE;

        // TODO: Widgetの初期設定
        pWidget->SetName(in_name);

        return handle;
    }

    const Bool UIModule::AddChildWidget(Core::Common::Handle in_hParent, Core::Common::Handle in_hWidget)
    {
#ifdef _HOBBY_ENGINE
        return HOBBY_CURRENT_LEVEL.AddParentActor(in_hWidget, in_hParent);
#else
        return FALSE;
#endif
    }
}
