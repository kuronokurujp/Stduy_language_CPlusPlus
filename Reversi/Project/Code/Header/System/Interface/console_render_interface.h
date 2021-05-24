// というかこれはレンダリングの共通インターフェイスにすべき
// win32/OpenGL/DirectXでも使えるように
#ifndef __CONSOLE_RENDER_INTERFACE_H__
#define __CONSOLE_RENDER_INTERFACE_H__

#include <string>

// 前方宣言
class RenderViewInterface;

/// <summary>
/// コンソールのレンダリングインターフェイス.
/// </summary>
class RenderingInterface
{
public:
	RenderingInterface() {}

	/// <summary>
	/// 描画インターフェイスの登録.
	/// </summary>
	virtual void AddViewInterface(RenderViewInterface* in_pAddRender) = 0;

	/// <summary>
	/// 描画インターフェイスの解除.
	/// </summary>
	virtual void RemoveViewInterface(RenderViewInterface* in_pRemoveRender) = 0;

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
		const float in_startPointX,
		const float in_startPointY,
		const unsigned int in_width,
		const unsigned int in_height) = 0;

	/// <summary>
	/// 描画する半角文字を1行単位で反映.
	/// </summary>
	virtual void FlashLineHalfCharacter(
		const char* in_pRectCharcterHalfSize,
		const float in_startPointX,
		const float in_startPointY) = 0;

	/// <summary>
	/// 描画.
	/// </summary>
	virtual void Draw() = 0;

	virtual void EndDraw() = 0;

private:
	void operator =(const RenderingInterface(&src)) {}
	RenderingInterface(const RenderingInterface(&src)) {}
};

/// <summary>
/// 描画インターフェイス.
/// </summary>
class RenderViewInterface
{
public:
	/// <summary>
	/// 矩形描画.
	/// </summary>
	/// <returns></returns>
	virtual void Draw(RenderingInterface* in_pRendering) = 0;
};

#endif // __CONSOLE_RENDER_INTERFACE_H__
