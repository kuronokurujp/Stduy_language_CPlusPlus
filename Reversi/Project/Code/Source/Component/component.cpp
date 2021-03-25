#include "Component/component.h"
#include "Actor/actor.h"

Component::Component(Actor* in_pOwner)
{
	this->_Clear();

	if (in_pOwner == NULL)
	{
		return;
	}

	this->_pOwner = in_pOwner;

	this->_pOwner->AddComponentMemData(this);
}

void Component::_Clear()
{
	this->_pOwner = NULL;
}