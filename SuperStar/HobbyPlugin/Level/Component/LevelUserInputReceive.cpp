#include "LevelUserInputReceive.h"

#include "HobbyPlugin/Actor/Actor.h"

namespace Level
{
    void LevelUserInputReceiveComponent::Setup(const Bool in_bAutoDelete)
    {
        Actor::Component::Setup(in_bAutoDelete);
    }

    void LevelUserInputReceiveComponent::Message(const Char* in_pMsg)
    {
        this->_eventReceiver->Message(in_pMsg);
    }

}  // namespace Level
