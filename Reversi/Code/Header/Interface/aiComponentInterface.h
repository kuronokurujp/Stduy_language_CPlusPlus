#ifndef __AI_COMPONENT_INTERFACE_H__
#define __AI_COMPONENT_INTERFACE_H__

class AIComponentInterface
{
public:
	enum eState
	{
		eState_None = 0,
		eState_Calculating,
		eState_SuccessPlacement,
		eState_SkipPlacement,
	};

	// 打ち手開始
	virtual void Start() = 0;

	// 現在の状態
	virtual const eState GetState() const = 0;
};

#endif // __AI_COMPONENT_INTERFACE_H__
