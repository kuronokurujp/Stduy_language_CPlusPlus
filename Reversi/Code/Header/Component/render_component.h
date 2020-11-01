#ifndef __RENDER_COMPONENT_H__
#define __RENDER_COMPONENT_H__

#include "Component/component.h"
#include "System/Interface/console_render_Interface.h"

class RenderComponent : public Component, public RenderViewInterface
{
public:
	RenderComponent(Actor* in_pOwner, RenderingInterface* in_pRendering);
	virtual ~RenderComponent();

	virtual void Update(const float in_deltaTimeSecond) override {}

	/// <summary>
	/// 描画.
	/// </summary>
	/// <returns></returns>
	virtual void Draw(RenderingInterface* in_pRendering) = 0;

private:
	void _Clear();

	RenderingInterface* _pRenderingInterface;
};

#endif // __RENDER_COMPONENT_H__