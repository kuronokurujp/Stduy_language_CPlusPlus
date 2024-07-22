#include "Widget.h"

#include "Component/Input/UIInputRouter.h"

namespace UI
{
    const Bool Widget::Begin()
    {
        if (Actor::Object::Begin() == FALSE) return FALSE;

        this->_inputHandle = this->GetComponentHandle(&UIInputRouterComponent::CLASS_RTTI);

        return TRUE;
    }

    const Bool Widget::End()
    {
        if (Actor::Object::End() == FALSE) return FALSE;

        return TRUE;
    }

    void Widget::OnAddChildActor(Actor::Object* in_pChildActor)
    {
        Actor::Object::OnAddChildActor(in_pChildActor);
    }

    void Widget::OnRemoveChildActor(Actor::Object* in_pChildActor)
    {
        Actor::Object::OnRemoveChildActor(in_pChildActor);
    }

    void Widget::_ProcessInput(const Float32 in_fDt, Platform::InputSystemInterface* in_pInput)
    {
        // 入力ルーターに入力情報を渡して子の入力端末の入力情報を更新
        HE_ASSERT(in_pInput != NULL);
        if (this->ValidComponent(this->_inputHandle))
        {
            UIInputRouterComponent* pInputRouter =
                this->GetComponent<UIInputRouterComponent>(this->_inputHandle);

            Core::Common::FastFixArray<Actor::Component*, 32> inputTerminalComArray;

            for (Uint32 i = 0; i < this->_aChildObject.Size(); ++i)
            {
                inputTerminalComArray.Clear();

                auto childObject = this->_aChildObject[i];
                childObject->OutputChildrenComponent(&inputTerminalComArray,
                                                     &UIInputIComponentInterface::CLASS_RTTI);
                pInputRouter->ProcessInput(in_pInput, inputTerminalComArray);
            }
        }
    }
}  // namespace UI
