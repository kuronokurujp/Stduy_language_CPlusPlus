﻿#include "LevelUserInputReceive.h"

#include "ActorModule.h"

namespace Level
{
    void LevelUserInputReceiveComponent::VSetup(const Bool in_bAutoDelete)
    {
        Actor::Component::VSetup(in_bAutoDelete);
    }

    void LevelUserInputReceiveComponent::Message(const Char* in_szMsg)
    {
        this->_spEventReceiver->Message(in_szMsg);
    }

}  // namespace Level
