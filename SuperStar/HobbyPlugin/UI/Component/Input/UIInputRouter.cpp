#include "UIInputRouter.h"

#include "HobbyPlugin/Actor/Actor.h"

namespace UI
{
    /// <summary>
    /// Processes the input.
    /// </summary>
    void UIInputRouterComponent::ProcessInput(
        Platform::InputSystemInterface* in_pInput,
        Core::Common::FastFixArrayBase<Actor::Component*>& in_arTerminal)
    {
        // TODO: UIに関わる, マウスのクリックやキーボードなどの結果を受け取り, 各入力端末へ通知する
        HE_ASSERT(this->_pOwner != NULL);
        for (Uint32 i = 0; i < in_arTerminal.Size(); ++i)
        {
            UIInputIComponentInterface* pTerminal =
                reinterpret_cast<UIInputIComponentInterface*>(in_arTerminal[i]);
            pTerminal->ProcessInput(in_pInput);
        }
    }
}  // namespace UI
