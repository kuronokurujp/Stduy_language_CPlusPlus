#include "Component/moveComponent.h"

#include <SDL/SDL.h>

#include "Actor/actor.h"
#include "Math/math.h"

/// <summary>
/// Initializes a new instance of the <see cref="MoveComponent"/> class.
/// </summary>
/// <param name="in_pOwner">The in p owner.</param>
/// <param name="in_updateOrder">The in update order.</param>
MoveComponent::MoveComponent(Actor* in_pOwner, const int in_updateOrder)
    : Component(in_pOwner, in_updateOrder)
{
}

/// <summary>
/// Updates the specified in delta time.
/// </summary>
/// <param name="in_deltaTime">The in delta time.</param>
/// <returns></returns>
auto MoveComponent::Update(const float in_deltaTime) -> void
{
    // 回転
    if (!Math::NearZero(this->angularSpeed))
    {
        auto rot = this->pOwner->GetRotation();

        auto speed = this->angularSpeed * in_deltaTime;
        Math::Quaternion inc(Math::Vector3::Unit_Z, speed);
        // クォータニオン同士の回転
        rot = Math::Quaternion::Concatenate(rot, inc);

        this->pOwner->SetRotation(rot);
    }

    // 向いている方向に移動
    if (!Math::NearZero(this->forwardSpeed))
    {
        Math::Vector3 pos     = this->pOwner->GetPosition();
        Math::Vector3 forward = this->pOwner->GetForward();
        Math::Vector3 move    = forward * (this->forwardSpeed * in_deltaTime);
        pos += move;

        this->pOwner->SetPosition(pos);
    }
}