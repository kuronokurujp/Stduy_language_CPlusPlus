#ifndef __INPUT__H__
#define __INPUT__H__

#include "SDL/SDL.h"

/// <summary>
/// 入力のインターフェイス
/// </summary>
class IInterface_Input
{
public:
	/// <summary>
	/// 指定idの入力をしているか.
	/// </summary>
	/// <param name="in_actionId">The in action identifier.</param>
	/// <returns></returns>
	virtual const bool IsPress(Uint32 in_actionId) = 0;

	/// <summary>
	/// Determines whether the specified in action identifier is repeat.
	/// </summary>
	/// <param name="in_actionId">The in action identifier.</param>
	/// <returns></returns>
	virtual const bool IsRepeat(Uint32 in_actionId) = 0;
};

/// <summary>
/// Windows用の入力インターフェイス
/// </summary>
/// <seealso cref="IInterface_Input" />
class Windows_Input : public IInterface_Input {
public:
	Windows_Input() { this->_Clear(); }

	/// <summary>
	/// 指定idの入力をしているか.
	/// </summary>
	/// <param name="in_actionId">The in action identifier.</param>
	/// <returns></returns>
	const bool IsPress(Uint32 in_actionId) override final
	{
		if (this->pState == nullptr)
		{
			return false;
		}

		return this->pState[in_actionId];
	}

	/// <summary>
	/// Determines whether the specified in action identifier is repeat.
	/// </summary>
	/// <param name="in_actionId">The in action identifier.</param>
	/// <returns></returns>
	const bool IsRepeat(Uint32 in_actionId) override final
	{
		// todo: 特定キーしか使えない(↑とか)、のちに対応
		if (this->pState == nullptr)
		{
			return false;
		}

		if (this->sdlEvent.type != SDL_KEYDOWN)
		{
			return false;
		}

		if (this->sdlEvent.key.repeat != 0)
		{
			return false;
		}

		if (this->sdlEvent.key.keysym.scancode != static_cast<SDL_Scancode>(in_actionId))
		{
			return false;
		}

		return this->pState[in_actionId];
	}

	/// <summary>
	/// Sets the state of the keyboard.
	/// </summary>
	/// <param name="in_pState">State of the in p.</param>
	auto SetKeyboardState(const Uint8 *in_pState, SDL_Event in_eventData)
		-> void {
		this->pState = in_pState;
		this->sdlEvent = in_eventData;
	}

private:
	/// <summary>
	/// Clears this instance.
	/// </summary>
	auto _Clear() -> void { this->pState = nullptr; }

	const Uint8 *pState;
	SDL_Event sdlEvent;
};

#endif // __INPUT__H__