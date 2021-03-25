// コンソール専用キーボード入力
#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include "System/Interface/keyboard_interface.h"
#include <string>

class Keyboard final : public KeyboardInterface
{
public:
	Keyboard();
	~Keyboard();

	/// <summary>
	/// キーボート入力.
	/// </summary>
	void Input();

	/// <summary>
	/// テキスト入力完了したか.
	/// </summary>
	bool IsConfirmText() override final { return this->_enterInputFlag; }

	/// <summary>
	/// 入力完了したテキスト取得.
	/// </summary>
	/// <returns></returns>
	const char* GetConfirmEnterText() override final;

	/// <summary>
	/// 入力中のテキスト取得.
	/// </summary>
	/// <returns></returns>
	const char* GetConfirmInputText() override final;

private:
	void _Clear();

	char* _pPushKeyCode;

	std::string _inputText;
	std::string _enterInputText;
	bool _enterInputFlag;

	// キーマップを作成
	static const char s_keyMap[];
};

#endif // __KEYBOARD_H__