#pragma once

#include "Component.h"
#include <memory>

// 前方宣言
namespace Actor { class ActorManager; }

namespace Component
{
    class ActorComponent : public Component
    {
    public:
        ActorComponent(Actor::ActorManager* in_p_actor_manager, Actor::Actor* in_p_owner);

        Actor::ActorManager* Get() const { return this->_p_actor_manager; }

    private:
        Actor::ActorManager* _p_actor_manager;
    };
}
