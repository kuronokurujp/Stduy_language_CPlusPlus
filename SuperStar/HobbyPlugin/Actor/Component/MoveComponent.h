#ifndef __MOVECOMPONENT_H__
#define __MOVECOMPONENT_H__

#include "component.h"

class Actor;

/// <summary>
/// Actor移動用
/// </summary>
/// <seealso cref="Component" />
class MoveComponent : public Component
{
public:
	/// <summary>
	/// Initializes a new instance of the <see cref="MoveComponent"/> class.
	/// </summary>
	/// <param name="in_pOwner">The in p owner.</param>
	/// <param name="in_updateOrder">The in update order.</param>
	MoveComponent(Actor* in_pOwner, const int in_updateOrder);

	/// <summary>
	/// Updates the specified in delta time.
	/// </summary>
	/// <param name="in_deltaTime">The in delta time.</param>
	/// <returns></returns>
	void Update(const float in_deltaTime) override final;

	/// <summary>
	/// Gets the angular speed.
	/// </summary>
	/// <returns></returns>
	const float GetAngularSpeed() { return this->angularSpeed; }

	/// <summary>
	/// Gets the forword speed.
	/// </summary>
	/// <returns></returns>
	const float GetForwardSpeed() { return this->forwardSpeed; }

	/// <summary>
	/// Sets the angular speed.
	/// </summary>
	/// <param name="in_angularSpeed">The in angular speed.</param>
	/// <returns></returns>
	void SetAngularSpeed(const float in_angularSpeed) { this->angularSpeed = in_angularSpeed; }

	/// <summary>
	/// Sets the forward speed.
	/// </summary>
	/// <param name="in_forwardSpeed">The in forward speed.</param>
	/// <returns></returns>
	void SetForwardSpeed(const float in_forwardSpeed) { this->forwardSpeed = in_forwardSpeed; }

private:
	/// <summary>
	/// Clears this instance.
	/// </summary>
	/// <returns></returns>
	void _Clear()
	{
		this->angularSpeed = 0.0f;
		this->forwardSpeed = 0.0f;
	}

	float angularSpeed;
	float forwardSpeed;
};

#endif // __MOVECOMPONENT_H__