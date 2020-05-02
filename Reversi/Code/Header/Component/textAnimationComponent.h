#ifndef __TEXT_ANIMATION_COMPONENT_H__
#define __TEXT_ANIMATION_COMPONENT_H__

#include "Component/component.h"

// 前方宣言
class Actor;

class TextAnimationComponent final : public Component
{
public:
	TextAnimationComponent(Actor* in_pActor);

	/// <summary>
	/// 毎フレーム更新.
	/// </summary>
	void Update(const float in_deltaTimeSecond) override final;

	void StartAnimation(const char* in_pHalfCharacterText, const bool in_loopFlag);
	void EndAnimation() { this->_animationFlag = false; }

	bool IsAnimation() const { return this->_animationFlag; }

	const char* GetText() const { return this->_aHalfCharacterText; }

private:
	void _Clear();

	bool _animationFlag;
	bool _loopFlag;
	unsigned int _textLength;
	unsigned int _textIndex;

	char _aHalfCharacterText[256];
};

#endif // __TEXT_ANIMATION_COMPONENT_H__
