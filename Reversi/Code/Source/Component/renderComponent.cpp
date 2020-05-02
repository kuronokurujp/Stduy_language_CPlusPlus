#include "Component/renderComponent.h"

#include "Interface/consoleRenderInterface.h"

RenderComponent::RenderComponent(Actor* in_pOwner, ConsoleRenderingInterface* in_pRendering)
	: Component(in_pOwner)
{
	this->_pRenderingInterface = in_pRendering;
	this->_pRenderingInterface->AddDrawInterface(this);
}

RenderComponent::~RenderComponent()
{
	this->_pRenderingInterface->RemoveDrawInterface(this);
}

void RenderComponent::_Clear()
{
	this->_pRenderingInterface = NULL;
}