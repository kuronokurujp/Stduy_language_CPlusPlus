#include "UIInputRouter.h"

#include "ActorModule.h"

namespace UI
{
    /// <summary>
    /// Processes the input.
    /// </summary>
    void UIInputRouterComponent::ProcessInput(
        Platform::InputSystemInterface* in_pInput,
        Core::Common::StackBase<Actor::Component*>& in_arTerminal)
    {
        // TODO: UIに関わる, マウスのクリックやキーボードなどの結果を受け取り, 各入力端末へ通知する
        HE_ASSERT(this->_pOwner != NULL);
        while (in_arTerminal.Empty() == FALSE)
        {
            UIInputIComponentInterface* pTerminal =
                reinterpret_cast<UIInputIComponentInterface*>(in_arTerminal.PopBack());
            pTerminal->ProcessInput(in_pInput);
        }
    }
}  // namespace UI
