#include "Component/textAnimationComponent.h"

#include "Actor/actor.h"
#include <stdlib.h>

TextAnimationComponent::TextAnimationComponent(Actor* in_pActor)
	: Component(in_pActor)
{
	this->_Clear();
}

/// <summary>
/// 毎フレーム更新.
/// </summary>
void TextAnimationComponent::Update(const float in_deltaTimeSecond)
{
	if (this->_animationFlag == false)
	{
		return;
	}

	if (this->_textIndex >= this->_textLength)
	{
		this->_animationFlag = false;
		return;
	}

	unsigned int endOfIndex = this->_textIndex + 1;
	const char nextHalfCharacter = this->_aHalfCharacterText[endOfIndex];

	this->_aHalfCharacterText[endOfIndex] = this->_aHalfCharacterText[this->_textIndex];
	this->_aHalfCharacterText[this->_textIndex] = nextHalfCharacter;

	++this->_textIndex;
	if (this->_loopFlag == true)
	{
		this->_textIndex = this->_textIndex % this->_textLength;
	}
}

void TextAnimationComponent::StartAnimation(const char* in_pHalfCharacterText, const bool in_loopFlag)
{
	this->_animationFlag = true;
	memcpy_s(&this->_aHalfCharacterText[1], sizeof(this->_aHalfCharacterText) - 1, in_pHalfCharacterText, strlen(in_pHalfCharacterText));
	this->_aHalfCharacterText[0] = '\0';

	this->_textIndex = 0;
	this->_textLength = static_cast<unsigned int>(strlen(&this->_aHalfCharacterText[1]));
	this->_loopFlag = in_loopFlag;
}

void TextAnimationComponent::_Clear()
{
	this->_animationFlag = false;
	this->_loopFlag = false;
	this->_textLength = 0;
	this->_textIndex = 0;

	memset(this->_aHalfCharacterText, 0, sizeof(this->_aHalfCharacterText));
}