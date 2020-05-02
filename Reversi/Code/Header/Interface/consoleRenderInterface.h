#ifndef __CONSOLE_RENDER_INTERFACE_H__
#define __CONSOLE_RENDER_INTERFACE_H__

#include <string>

// 前方宣言
class ConsoleDrawInterface;

/// <summary>
/// コンソールのレンダリングインターフェイス.
/// </summary>
class ConsoleRenderingInterface
{
public:
	/// <summary>
	/// 描画インターフェイスの登録.
	/// </summary>
	virtual void AddDrawInterface(ConsoleDrawInterface* in_pAddRender) = 0;

	/// <summary>
	/// 描画インターフェイスの解除.
	/// </summary>
	virtual void RemoveDrawInterface(ConsoleDrawInterface* in_pRemoveRender) = 0;

	// レンダーのテキスト縦横サイズ
	virtual const unsigned GetTextRenderWidth() = 0;
	virtual const unsigned GetTextRenderHeight() = 0;

	// 画面クリア文字コード
	virtual const char GetClsCharacteCode() = 0;

	/// <summary>
	/// 描画する半角文字を反映.
	/// </summary>
	virtual void FlashRectHalfCharacter(
		const char* in_pRectCharcterHalfSize,
		const int in_startPointX,
		const int in_startPointY,
		const unsigned int in_width,
		const unsigned int in_height) = 0;

	/// <summary>
	/// 描画する半角文字を1行単位で反映.
	/// </summary>
	virtual void FlashLineHalfCharacter(
		const char* in_pRectCharcterHalfSize,
		const int in_startPointX,
		const int in_startPointY) = 0;
};

/// <summary>
/// コンソールの描画インターフェイス.
/// </summary>
class ConsoleDrawInterface
{
public:
	/// <summary>
	/// 矩形描画.
	/// </summary>
	/// <returns></returns>
	virtual void Draw(ConsoleRenderingInterface* in_pRendering) = 0;
};

#endif // __CONSOLE_RENDER_INTERFACE_H__
