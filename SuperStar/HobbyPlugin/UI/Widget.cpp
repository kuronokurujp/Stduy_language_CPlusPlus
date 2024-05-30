#include "Widget.h"

#include "Component/Input/UIInputRouter.h"

namespace UI
{
    void Widget::OnAddChildActor(Actor::Object* in_pChildActor)
    {
        Actor::Object::OnAddChildActor(in_pChildActor);

        // TODO: 親アクターに設定しているInputRouterに追加設定
        UIInputRouterComponent* pInputRouter = reinterpret_cast<UIInputRouterComponent*>(this->GetComponent(UIInputRouterComponent::CLASS_RTTI));
        if (pInputRouter == NULL)
            return;

        Core::Common::FastFixArray<Actor::Component*, 128> inputTerminalComArray;
        in_pChildActor->OutputChildComponents(inputTerminalComArray, UIInputIComponentInterface::CLASS_RTTI);
        for (Uint32 i = 0; i < inputTerminalComArray.Size(); ++i)
        {
            pInputRouter->AddTerminal(reinterpret_cast<UIInputIComponentInterface*>(inputTerminalComArray[i]));
        }
    }

    void Widget::OnRemoveChildActor(Actor::Object* in_pChildActor)
    {
        // TODO: アクターに設定しているInputRouterの解除
        UIInputRouterComponent* pInputRouter = reinterpret_cast<UIInputRouterComponent*>(this->GetComponent(UIInputRouterComponent::CLASS_RTTI));
        if (pInputRouter != NULL)
        {
            Core::Common::FastFixArray<Actor::Component*, 128> inputTerminalComArray;
            in_pChildActor->OutputChildComponents(inputTerminalComArray, UIInputIComponentInterface::CLASS_RTTI);
            for (Uint32 i = 0; i < inputTerminalComArray.Size(); ++i)
            {
                pInputRouter->RemoveTeminal(reinterpret_cast<UIInputIComponentInterface*>(inputTerminalComArray[i]));
            }
        }

        Actor::Object::OnRemoveChildActor(in_pChildActor);
    }

    void Widget::_ProcessInput(const Float32 in_dt, Platform::InputSystemInterface* in_pInput)
    {
        // TODO: 入力ルーターに入力情報を渡して子の入力端末の入力情報を更新
        if (this->_hInputRouter.Null())
            return;

        {
            E_ASSERT(in_pInput != NULL);

            UIInputRouterComponent* pInputRouter = reinterpret_cast<UIInputRouterComponent*>(this->GetComponent(UIInputRouterComponent::CLASS_RTTI));
            E_ASSERT(pInputRouter != NULL);
            pInputRouter->ProcessInput(in_pInput);
        }
    }
}
