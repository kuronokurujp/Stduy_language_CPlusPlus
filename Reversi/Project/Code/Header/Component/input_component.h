#ifndef __INPUT_COMPONENT_H__
#define __INPUT_COMPONENT_H__

#include <functional>
#include <vector>

#include "Component/component.h"

/// <summary>
/// 入力コンポーネント
/// </summary>
/// <seealso cref="Component" />
class InputComponent : public Component
{
public:
	// タッチイベント
	enum eTouchEvent
	{
		TOUCH_EVENT_MODEL = 1,
	};

	// タッチイベントデータ
	union _touch_event_data_
	{
		struct _touch_event_model_
		{
			unsigned long handle;
		} _touch_model;
	};

	InputComponent(Actor* in_pOwner) : Component(in_pOwner) {}
	InputComponent(
		Actor* in_pOwner,
		std::function<void(Actor*, const eTouchEvent, std::vector<_touch_event_data_>&)> in_event)
		: Component(in_pOwner) {
		this->_touch_event_to_actor = in_event;
	}

	virtual ~InputComponent() {}

	virtual void Update(const float in_deltaTimeSecond) override {}

	virtual void OnTouchEvent(
		const eTouchEvent in_type, std::vector<_touch_event_data_>& in_datas)
	{
		// TODO: ラムダ式のイベント変数があれば呼び出す
		this->_touch_event_to_actor(this->_pOwner, in_type, in_datas);
	}

private:
	std::function<void(Actor*, const eTouchEvent, std::vector<_touch_event_data_>&)> _touch_event_to_actor;
};

#endif // __INPUT_COMPONENT_H__
