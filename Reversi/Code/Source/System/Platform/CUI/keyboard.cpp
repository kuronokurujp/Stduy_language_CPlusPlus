#include "System/Platform/CUI/Keyboard.h"

#include <Windows.h>
#include <cctype>

#include "common.h"

enum eKeyCode
{
	eKeyCode_Enter = 0x0d,
	eKeyCode_BackSpace = 0x08,
};

// 使用するキーコードのみ用意
const char Keyboard::s_keyMap[] =
{
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'J', 'K',
	'R', 'U',
	eKeyCode_Enter, eKeyCode_BackSpace,
};

Keyboard::Keyboard()
{
	this->_Clear();

	// 文字列クラスのバッファサイズを固定
	this->_inputText.reserve(32);
	this->_enterInputText.reserve(this->_inputText.capacity());

	this->_pPushKeyCode = new char[StaticSingleArrayLength(Keyboard::s_keyMap)];
	memset(this->_pPushKeyCode, 0, sizeof(char) * StaticSingleArrayLength(Keyboard::s_keyMap));
}

Keyboard::~Keyboard()
{
	SAFETY_MEM_ARRAY_RELEASE(this->_pPushKeyCode);
}

/// <summary>
/// キーボート入力.
/// </summary>
void Keyboard::Input()
{
	this->_enterInputFlag = false;
	// キーボードのキーを入力しているか検出
	{
		for (unsigned int i = 0; i < StaticSingleArrayLength(Keyboard::s_keyMap); ++i)
		{
			auto keyCode = Keyboard::s_keyMap[i];
			int state = GetAsyncKeyState(keyCode);
			if (state & 0x8000)
			{
				// 連続に押しても反応しないようにする
				if (this->_pPushKeyCode[i] != keyCode)
				{
					this->_pPushKeyCode[i] = keyCode;

					switch (keyCode)
					{
					case eKeyCode_Enter:
					{
						this->_enterInputFlag = true;
						break;
					}
					case eKeyCode_BackSpace:
					{
						if (this->_inputText.size() > 0)
						{
							this->_inputText.pop_back();
						}
						break;
					}
					default:
					{
						if (isalpha(keyCode))
						{
							// 大文字を小文字にする
							keyCode += 32;
						}

						this->_inputText.push_back(keyCode);
						break;
					}
					}
				}

				break;
			}
			// 押していない
			else
			{
				if (this->_pPushKeyCode[i] == keyCode)
				{
					this->_pPushKeyCode[i] = 0;
				}
			}
		}
	}

	// Enterキー入力に成功したら入力終了として入力文字列として設定
	if (this->_enterInputFlag == true)
	{
		this->_enterInputText = this->_inputText;
		this->_inputText.clear();
	}
}

const char* Keyboard::GetConfirmEnterText()
{
	return this->_enterInputText.c_str();
}

const char* Keyboard::GetConfirmInputText()
{
	return this->_inputText.c_str();
}

void Keyboard::_Clear()
{
	this->_pPushKeyCode = NULL;
	this->_inputText.clear();
	this->_enterInputText.clear();
	this->_enterInputFlag = false;
}