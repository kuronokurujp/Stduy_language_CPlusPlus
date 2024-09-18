#include "PlatformInput.h"

namespace Platform
{
    EInputState KeyboardInput::GetKeyState(const EKeyboard in_eKeyCode) const
    {
        if (this->_aPrevState[in_eKeyCode] == EInputState::EInputState_NONE)
        {
            if (this->_aCurrState[in_eKeyCode] == EInputState::EInputState_NONE)
            {
                return EInputState::EInputState_NONE;
            }
            else
            {
                return EInputState::EInputState_PRESSED;
            }
        }
        else
        {
            if (this->_aCurrState[in_eKeyCode] == EInputState::EInputState_NONE)
            {
                return EInputState::EInputState_RELEASED;
            }
            else
            {
                return EInputState::EInputState_HOLD;
            }
        }

        return EInputState::EInputState_NONE;
    }

    Bool TouchInput::GetTouchValue(const EInputMouseType in_type) const
    {
        return ((this->_uCurrTouchState & in_type) != 0);
    }

    EInputState TouchInput::GetTouchState(const EInputMouseType in_type) const
    {
        if ((this->_uPrevTouchState & in_type) == 0)
        {
            if ((this->_uCurrTouchState & in_type) == 0)
            {
                return EInputState::EInputState_NONE;
            }
            else
            {
                return EInputState::EInputState_PRESSED;
            }
        }
        else
        {
            if ((this->_uCurrTouchState & in_type))
            {
                return EInputState::EInputState_RELEASED;
            }
            else
            {
                return EInputState::EInputState_HOLD;
            }
        }

        return EInputState::EInputState_NONE;
    }

    Bool TouchInput::IsTouchInRect(const Core::Math::Rect2& in_rRect) const
    {
        return in_rRect.InSidePoint(this->GetWorldPos());
    }
}  // namespace Platform
