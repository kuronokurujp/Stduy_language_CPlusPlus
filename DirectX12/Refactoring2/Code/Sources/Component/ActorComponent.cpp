#include "Component/ActorComponent.h"

#include "Actor/ActorManager.h"

namespace Component
{
    ActorComponent::ActorComponent(Actor::ActorManager* in_p_actor_manager, Actor::Actor* in_p_owner)
        : Component(in_p_owner)
    {
        this->_p_actor_manager = in_p_actor_manager;
    }
}
