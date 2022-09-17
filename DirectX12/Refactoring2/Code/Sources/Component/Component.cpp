#include "Component/component.h"
#include "Actor/actor.h"

namespace Component
{
    Component::Component(Actor::Actor* in_pOwner)
    {
        this->_Clear();

        if (in_pOwner == NULL)
        {
            return;
        }

        this->_pOwner = in_pOwner;

        this->_pOwner->AddComponentMemData(std::shared_ptr<Component>(this));
    }

    void Component::_Clear()
    {
        this->_pOwner = NULL;
    }
}
