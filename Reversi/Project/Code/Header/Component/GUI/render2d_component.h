#ifndef __RENDER2D_COMPONENT_H__
#define __RENDER2D_COMPONENT_H__

// TODO: 2Dで描画するコンポーネント
#include "Component/render_component.h"

class Render2DComponent final : public RenderComponent
{
public:
	Render2DComponent(Actor* in_p_actor, RenderingInterface* in_p_rendering) :
		RenderComponent(in_p_actor, in_p_rendering) {}
	~Render2DComponent() {}

	/// <summary>
	/// 矩形描画.
	/// </summary>
	/// <returns></returns>
	void Draw(RenderingInterface* in_pRendering) override final {};

private:
};

#endif // __RENDER2D_COMPONENT_H__
