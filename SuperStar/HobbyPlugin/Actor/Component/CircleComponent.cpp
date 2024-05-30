#include "Engine/Headers/Component/circleComponent.h"
#include "Engine/Headers/Actor/actor.h"

/// <summary>
/// Initializes a new instance of the <see cref="CircleComponent"/> class.
/// </summary>
/// <param name="in_pOwner">The in p owner.</param>
/// <param name="in_updateOrder">The in update order.</param>
CircleComponent::CircleComponent(Actor* in_pOwner, const int in_updateOrder) :
	Component(in_pOwner, in_updateOrder)
{
}

/// <summary>
/// Updates the specified in delta time.
/// </summary>
/// <param name="in_deltaTime">The in delta time.</param>
/// <returns></returns>
auto CircleComponent::Update(const float /*in_deltaTime*/) -> void
{
}

/// <summary>
/// Gets the center.
/// </summary>
/// <returns></returns>
auto CircleComponent::GetCenterPosition() -> const Math::Vector3&
{
	return this->pOwner->GetPosition();
}

/// <summary>
/// Intersects the specified in p a.
/// </summary>
/// <param name="in_rA">The in r a.</param>
/// <param name="in_rB">The in r b.</param>
/// <returns></returns>
auto CircleComponent::Intersect(CircleComponent& in_rA, CircleComponent& in_rB) -> const bool
{
	// お互いの距離がお互いの半径の合計より小さいのであれば当たっている
	Math::Vector3 diff;
	diff.SetSub(in_rA.GetCenterPosition(), in_rB.GetCenterPosition());

	const float distSq = Math::Vector3::GetLengthSquared(diff);

	float radiusSq = in_rA.GetRadius() + in_rB.GetRadius();
	radiusSq *= radiusSq;

	return distSq <= radiusSq;
}