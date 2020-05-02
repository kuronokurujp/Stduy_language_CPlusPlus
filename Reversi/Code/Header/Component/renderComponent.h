#ifndef __RENDER_COMPONENT_H__
#define __RENDER_COMPONENT_H__

#include "Component/component.h"
#include "Interface/consoleRenderInterface.h"

class RenderComponent : public Component, public ConsoleDrawInterface
{
public:
	RenderComponent(Actor* in_pOwner, ConsoleRenderingInterface* in_pRendering);
	virtual ~RenderComponent();

	virtual void Update(const float in_deltaTimeSecond) override {}

	/// <summary>
	/// 描画.
	/// </summary>
	/// <returns></returns>
	virtual void Draw(ConsoleRenderingInterface* in_pRendering) = 0;

private:
	void _Clear();

	ConsoleRenderingInterface* _pRenderingInterface;
};

#endif // __RENDER_COMPONENT_H__