﻿#include "Component/component.h"

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
    }

    void Component::_Clear()
    {
        this->_pOwner = NULL;
    }
}