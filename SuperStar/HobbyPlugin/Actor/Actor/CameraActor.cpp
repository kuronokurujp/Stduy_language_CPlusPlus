#include "Actor/cameraActor.h"

#include "Component/moveComponent.h"
#include "Math/math.h"
#include "Math/matrix4.h"
#include "Math/vector3.h"
#include "renderer.h"

CameraActor::CameraActor(Renderer* in_pRenderer, ActorManager* in_pActorManager)
    : Actor(in_pRenderer, in_pActorManager)
{
    this->pMoveComp = new MoveComponent(this, 0);
}

void CameraActor::UpdateActor(float /*in_deltaTime*/)
{
    auto pRender = this->GetRenderer();

    Math::Vector3 evePosition    = this->GetPosition();
    Math::Vector3 targerPosition = evePosition + (this->GetForward() * 100.0f);
    Math::Vector3 upPosition     = Math::Vector3::Unit_Z;

    const Math::Matrix4 viewMtx =
        Math::Matrix4::CreateLookAt(evePosition, targerPosition, upPosition);
    pRender->SetViewMatrix(viewMtx);
}

void CameraActor::ActorInput(const uint8_t* keys)
{
    float forwardSpeed = 0.0f;
    float angularSpeed = 0.0f;
    // wasd movement
    if (keys[SDL_SCANCODE_W])
    {
        forwardSpeed += 300.0f;
    }
    if (keys[SDL_SCANCODE_S])
    {
        forwardSpeed -= 300.0f;
    }
    if (keys[SDL_SCANCODE_A])
    {
        angularSpeed -= Math::TwoPI;
    }
    if (keys[SDL_SCANCODE_D])
    {
        angularSpeed += Math::TwoPI;
    }

    this->pMoveComp->SetForwardSpeed(forwardSpeed);
    this->pMoveComp->SetAngularSpeed(angularSpeed);
}