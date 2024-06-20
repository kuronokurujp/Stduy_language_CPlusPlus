#ifndef __CANERA_ACTOR_H__
#define __CANERA_ACTOR_H__

#include "Actor/actor.h"

class Renderer;
class ActorManager;
class MoveComponent;

class CameraActor : public Actor
{
public:
    CameraActor(Renderer* in_pRenderer, ActorManager* in_pActorManager);

    void UpdateActor(float in_deltaTime) override final;
    void ActorInput(const uint8_t* keys) override final;

private:
    void _Clear() { this->pMoveComp = nullptr; }

    MoveComponent* pMoveComp;
};

#endif  // __CANERA_ACTOR_H__
