#include "PlatformInput.h"

namespace Platform
{
    const EInputState KeyboardInput::GetKeyState(const EKeyboard in_keyCode) const
    {
        if (this->_prevState[in_keyCode] == EInputState::EInputState_NONE)
        {
            if (this->_currState[in_keyCode] == EInputState::EInputState_NONE)
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
            if (this->_currState[in_keyCode] == EInputState::EInputState_NONE)
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

    const Bool TouchInput::GetTouchValue(const EInputMouseType in_type) const
    {
        return ((this->_currTouchState & in_type) != 0);
    }

    const EInputState TouchInput::GetTouchState(const EInputMouseType in_type) const
    {
        if ((this->_prevTouchState & in_type) == 0)
        {
            if ((this->_currTouchState & in_type) == 0)
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
            if ((this->_currTouchState & in_type))
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

    const Bool TouchInput::IsTouchInRect(const Core::Math::Rect2 in_rect) const
    {
        return in_rect.InSidePoint(this->GetWorldPos());
    }
}  // namespace Platform
