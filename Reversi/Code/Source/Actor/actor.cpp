#include "Actor/Actor.h"

#include "Component/component.h"
#include "common.h"

Actor::Actor()
{
	this->_Clear();
}

Actor::~Actor()
{
	// くっついているコンポーネントをすべて破棄する
	this->RemoveComponentAndMemFree();
}

void Actor::Update(float in_deltaTimeSecond)
{
	if (this->_state != eState_Active)
	{
		return;
	}

	// くっつているコンポーネント更新
	for (auto component : this->_components)
	{
		component->Update(in_deltaTimeSecond);
	}

	// コンポーネントがすべて更新してから実行
	// コンポーネントの結果が同フレーム取れる
	this->UpdateActor(in_deltaTimeSecond);
}

void Actor::AddComponentMemData(Component* in_pComponent)
{
	auto iter = this->_components.begin();
	this->_components.insert(iter, in_pComponent);
}

bool Actor::RemoveComponentAndMemFree(Component* in_pComponent)
{
	if (in_pComponent == NULL)
	{
		return false;
	}

	auto iter = std::find(
		this->_components.begin(),
		this->_components.end(),
		in_pComponent);
	if (iter == this->_components.end())
	{
		return false;
	}

	this->_components.erase(iter);
	SAFETY_MEM_RELEASE(in_pComponent);

	return true;
}

/// <summary>
/// くっつている全てのコンポーネント削除.
/// </summary>
/// <returns></returns>
void Actor::RemoveComponentAndMemFree()
{
	for (auto component : this->_components)
	{
		SAFETY_MEM_RELEASE(component);
	}

	this->_components.clear();
}

void Actor::_Clear()
{
	this->_state = eState_Active;
	this->_components.clear();
}