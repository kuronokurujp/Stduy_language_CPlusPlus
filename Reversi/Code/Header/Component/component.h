#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include <stdio.h>
#include <cstdint>

// 前方宣言
class Actor;

/// <summary>
/// Actorクラスにくっつけるコンポーネント.
/// </summary>
class Component
{
public:
	Component(Actor* in_pOwner);
	virtual ~Component() {}

	/// <summary>
	/// 毎フレーム更新.
	/// </summary>
	virtual void Update(const float in_deltaTimeSecond) = 0;

protected:
	Actor* _pOwner;

private:
	void _Clear();
};

#endif // __COMPONENT_H__