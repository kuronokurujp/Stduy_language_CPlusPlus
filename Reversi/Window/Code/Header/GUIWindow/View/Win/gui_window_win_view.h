#ifndef __GUI_WINDOW_WIN_VIEW_H__
#define __GUI_WINDOW_WIN_VIEW_H__

#include <windows.h>
#include <wingdi.h>
#include <tchar.h>
#include <chrono>
#include <memory>
#include <stdio.h>

class GUIWindowController;
class GUIWindowModel;

// WinAPIを使って生成されたグラフィックスウィンドウView
class GUIWindowWinView
{
public:
	GUIWindowWinView() {}

	void CreateObject(GUIWindowController* in_p_ctr);
	~GUIWindowWinView();

	// Windowの描画内容書き換えをする
	void WriteRender(std::shared_ptr<GUIWindowModel> in_model);

	// メインウィンドウイベント処理
	static LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	void _Clear()
	{
		this->_p_ctrl = NULL;
	}

	GUIWindowController* _p_ctrl;
	HWND _h_wnd;
};

#endif // __GUI_WINDOW_WIN_VIEW_H__
