#include "Component/render_component.h"

#include "System/Interface/console_render_interface.h"

RenderComponent::RenderComponent(Actor* in_pOwner, RenderingInterface* in_pRendering)
	: Component(in_pOwner)
{
	this->_pRenderingInterface = in_pRendering;
	this->_pRenderingInterface->AddViewInterface(this);
}

RenderComponent::~RenderComponent()
{
	this->_pRenderingInterface->RemoveViewInterface(this);
}

void RenderComponent::_Clear()
{
	this->_pRenderingInterface = NULL;
}