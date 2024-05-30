#ifndef __WINDOW_H__
#define __WINDOW_H__

#include "common.h"

#include "SDL/SDL.h"

class IInterface_Input;
class Renderer;
class ActorManager;
class IInterface_PublicEngine;

/// <summary>
/// 画面のウィンドウデータ
/// </summary>
struct WindowConfigData
{
	const char *pTitle;
	int x, y, w, h;
	int topX, topY;
	int bottomX, bottomY;
	Color32 screenColor;
};

/// <summary>
/// ウィンドウインスタンスのインターフェイス
/// 描画機能、入力機能、コアシステムとの橋渡しなどが出来る
/// </summary>
class IInterface_Window
{
public:
	virtual inline const WindowConfigData& GetReferenceConfigData() = 0;
	virtual bool Load() = 0;

	/// <summary>
	/// Attaches the frame.
	/// </summary>
	/// <param name="in_pGameFrame">The in p game frame.</param>
	/// <param name="in_bResetFrame">The in b reset frame.</param>
	/// <returns></returns>
	virtual const bool AttachFrame(class IInterface_GameFrame *in_pGameFrame, const bool in_bResetFrame) = 0;

	/// <summary>
	/// Detaches the frame.
	/// </summary>
	/// <returns></returns>
	virtual void DetachFrame() = 0;

	/// <summary>
	/// Determines whether this instance is close.
	/// </summary>
	/// <returns>
	///   <c>true</c> if this instance is close; otherwise, <c>false</c>.
	/// </returns>
	virtual const bool IsClose() = 0;

	/// <summary>
	/// Closes this instance.
	/// </summary>
	virtual void Close() = 0;

	/// <summary>
	/// Renders the fill rect.
	/// </summary>
	/// <param name="in_rect">The in rect.</param>
	virtual void RenderFillRect(const SDL_Rect &in_rect) = 0;

	/// <summary>
	/// Gets the input.
	/// </summary>
	/// <returns></returns>
	virtual IInterface_Input* GetInput() = 0;

	virtual Renderer* GetRenderer() = 0;
	virtual ActorManager* GetActorManager() = 0;

	/// <summary>
	/// Gets the game.
	/// </summary>
	/// <returns></returns>
	virtual IInterface_PublicEngine* GetGame() = 0;
};

#endif // __WINDOW_H__