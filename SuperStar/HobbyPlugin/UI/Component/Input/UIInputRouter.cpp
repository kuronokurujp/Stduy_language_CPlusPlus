#include "UIInputRouter.h"

#include "HobbyPlugin/Actor/Actor.h"

namespace UI
{
    GENERATED_CLASS_BODY(UIInputRouterComponent, UIInputIComponentInterface)

    void UIInputRouterComponent::AddTerminal(UIInputIComponentInterface* in_pTerminal)
    {
        E_ASSERT(in_pTerminal != NULL);
        this->_terminals.PushBack(in_pTerminal);
    }

    void UIInputRouterComponent::RemoveTeminal(UIInputIComponentInterface* in_pTerminal)
    {
        E_ASSERT(in_pTerminal != NULL);
        this->_terminals.Remove(in_pTerminal);
    }

    /// <summary>
    /// Processes the input.
    /// </summary>
    void UIInputRouterComponent::ProcessInput(Platform::InputSystemInterface* in_pInput)
    {
        // TODO: UIに関わる, マウスのクリックやキーボードなどの結果を受け取り, 各入力端末へ通知する
        E_ASSERT(this->_pOwner != NULL);
        for (Uint32 i = 0; i < this->_terminals.Size(); ++i)
        {
            UIInputIComponentInterface* pTerminal = this->_terminals[i];
            pTerminal->ProcessInput(in_pInput);
        }
    }
}
