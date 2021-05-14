#ifndef __PLAYER_COMPONENT_H__
#define __PLAYER_COMPONENT_H__

#include "Component/component.h"
#include <string>

// 前方宣言
class KeyboardInterface;
class TextAnimationComponent;
class PlayerInputComponent;

class PlayerComponent final : public Component
{
public:
	PlayerComponent(Actor* in_pOwner, KeyboardInterface* in_pKeyboard);
	~PlayerComponent();

	void Update(const float in_deltaTimeSecond) override final;

private:
	enum eState
	{
		eState_InputText = 0,
		eState_TurnBegin,
		eState_TurnEnd,
		eState_ErrorMessage,
	};

	void _Clear();

	eState _state;
	KeyboardInterface* _p_keyboard;
	TextAnimationComponent* _p_textAnimationComponent;
	PlayerInputComponent* _p_inputComponent;
};

#endif // __PLAYER_COMPONENT_H__