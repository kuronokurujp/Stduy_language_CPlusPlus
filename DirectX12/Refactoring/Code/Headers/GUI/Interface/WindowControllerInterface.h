#pragma once

#include <memory>

namespace GUI
{
	/// <summary>
	/// ウィンドウ共通データ
	/// </summary>
	struct WindowCommonData
	{
		// ウィンドウのサイズ
		unsigned int width;
		unsigned int height;
	};

	enum eTouchEvent
	{
		TOUCH_EVENT_L_CLICK = 1,
	};

	// GUIWindowの制御インターフェイス
	class WindowControllerInterface
	{
	public:
		// 制御開始!
		virtual const bool Start() = 0;

		// 制御終了
		virtual void End() = 0;

		virtual void PostUpdate() = 0;

		// 更新の開始と終了
		virtual void BeginRender() = 0;
		virtual void EndRender() = 0;

		// 更新終了
		virtual const bool IsUpdate() = 0;

		// タッチイベント
		virtual void OnTouchEvent(int in_type, const int in_x, const int in_y) = 0;
		// アプリ終了イベント
		virtual void OnAppQuitEvent() = 0;

		virtual const WindowCommonData& GetWindowData() = 0;
	};
}
