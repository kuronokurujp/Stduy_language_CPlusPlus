#include "InGameSystemComponent.h"

namespace InGame
{
    void InGameSystemComponent::_Clear()
    {
        //::memset(&this->_actorHandleData, 0, sizeof(this->_actorHandleData));
        this->_bGameEnd = false;
        this->_uPoint   = 0;
    }
}  // namespace InGame
