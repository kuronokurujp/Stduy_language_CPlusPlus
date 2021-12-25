#pragma once

#include <windows.h>
#include <wingdi.h>

namespace GUI
{
	class WindowControllerInterface;
	class WindowModelInterface;

	// WinAPIを使って生成されたグラフィックスウィンドウView
	class WindowView
	{
	public:
		WindowView() { this->_Clear(); }
		~WindowView();

		/// <summary>
		/// ウィンドウオブジェクトを生成
		/// </summary>
		/// <param name="in_p_ctr"></param>
		virtual void CreateObject(WindowControllerInterface* in_p_ctr);

		void Show();

		const bool PostUpdate();

		// メインウィンドウイベント処理
		static LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		// 描画開始(描画に必要な前処理をする)
		virtual void BeginRender() {}

		// 描画終了(描画内容を画面へ反映)
		virtual void EndRender() {}

	protected:
		HWND _h_wnd;
		WindowControllerInterface* _p_ctrl;

	private:
		static void PushViewFromCallbackList(WindowView* in_p_view);
		static const bool PopViewFromList(WindowView* in_p_view);
		// ウィンドウハンドルと対応したViewを取得
		static WindowView* SearchViewFromList(HWND in_h_wnd);

		void _Clear()
		{
			this->_p_ctrl = nullptr;
			this->_h_wnd = 0;
			this->_msg = {};
			this->_wnd_class = {};
		}

		MSG _msg;
		WNDCLASSEX _wnd_class;
	};
}
