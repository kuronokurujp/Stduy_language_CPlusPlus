#ifndef __CIRCLE_COMPONENT_H__
#define __CIRCLE_COMPONENT_H__

#include "Component/component.h"
#include "Math/vector3.h"

class Actor;

/// <summary>
/// 円形上の属性コンポーネント.
/// todo 画面上に当たり判定を視覚化しないと.
/// </summary>
/// <seealso cref="Component" />
class CircleComponent : public Component
{
public:
    /// <summary>
    /// Initializes a new instance of the <see cref="CircleComponent"/> class.
    /// </summary>
    /// <param name="in_pOwner">The in p owner.</param>
    /// <param name="in_updateOrder">The in update order.</param>
    CircleComponent(Actor* in_pOwner, const int in_updateOrder);

    /// <summary>
    /// Updates the specified in delta time.
    /// </summary>
    /// <param name="in_deltaTime">The in delta time.</param>
    /// <returns></returns>
    void Update(const float in_deltaTime) override final;

    /// <summary>
    /// Sets the radius.
    /// </summary>
    /// <param name="in_radius">The in radius.</param>
    /// <returns></returns>
    void SetRadius(const float in_radius) { this->radius = in_radius; }

    /// <summary>
    /// Gets the radius.
    /// </summary>
    /// <returns></returns>
    const float GetRadius() const { return this->radius; }

    /// <summary>
    /// Gets the center.
    /// </summary>
    /// <returns></returns>
    const Math::Vector3& GetCenterPosition();

    /// <summary>
    /// Intersects the specified in p a.
    /// </summary>
    /// <param name="in_rA">The in r a.</param>
    /// <param name="in_rB">The in r b.</param>
    /// <returns></returns>
    static const bool Intersect(CircleComponent& in_rA, CircleComponent& in_rB);

private:
    /// <summary>
    /// Clears this instance.
    /// </summary>
    /// <returns></returns>
    void _Clear() { this->radius = 0.0f; }

    float radius;
};

#endif  // __CIRCLE_COMPONENT_H__