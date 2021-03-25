#ifndef __KEYBOARD_INTERFACE_H__
#define __KEYBOARD_INTERFACE_H__

/// <summary>
/// キーボードインターフェイス.
/// </summary>
class KeyboardInterface
{
public:
	/// <summary>
	/// テキスト入力完了したか.
	/// </summary>
	virtual bool IsConfirmText() = 0;

	/// <summary>
	/// 入力完了したテキスト取得.
	/// </summary>
	/// <returns></returns>
	virtual const char* GetConfirmEnterText() = 0;

	/// <summary>
	/// 入力中のテキスト取得.
	/// </summary>
	/// <returns></returns>
	virtual const char* GetConfirmInputText() = 0;
};

#endif // __KEYBOARD_INTERFACE_H__
