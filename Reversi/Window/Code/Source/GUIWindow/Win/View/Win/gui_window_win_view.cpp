#include "Window/Code/Header/GUIWindow/View/Win/gui_window_win_view.h"

#include "Window/Code/Header/GUIWindow/gui_window_controller.h"
#include "Window/Code/Header/GUIWindow/Model/gui_window_model.h"

#include <assert.h>

#include <vector>

static std::vector<GUIWindowWinView*> s_window_view_list;

void GUIWindowWinView::CreateObject(GUIWindowController* in_p_ctr)
{
	assert(in_p_ctr != NULL);

	this->_p_ctrl = in_p_ctr;
	auto model = this->_p_ctrl->GetModel();

	MSG msg;

	// アプリのインスタンス
	HINSTANCE h_instance = ::GetModuleHandle(NULL);

	// クラス名称
	const TCHAR* cp_class_name = _T("MainWindowClass");

	// メニュー
	const TCHAR* cp_menu = MAKEINTRESOURCE(NULL);

	// ウィンドウのタイトル名
	const TCHAR* cp_window_name = _T("テスト");

	// ウィンドウクラスのパラメータ設定
	WNDCLASSEX wnd_class;
	{
		wnd_class.cbSize = sizeof(WNDCLASSEX);
		wnd_class.style = CS_HREDRAW | CS_VREDRAW;
		wnd_class.lpfnWndProc = MainWindowProc;
		wnd_class.cbClsExtra = 0;
		wnd_class.cbWndExtra = 0;
		wnd_class.hInstance = h_instance;
		wnd_class.hIcon = ::LoadIcon(NULL, IDI_APPLICATION);
		wnd_class.hCursor = ::LoadCursor(NULL, IDC_ARROW);
		wnd_class.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wnd_class.lpszMenuName = cp_menu;
		wnd_class.lpszClassName = cp_class_name;
		wnd_class.hIconSm = NULL;
	}

	// ウィンドウクラス生成
	{
		if (0 == ::RegisterClassEx(&wnd_class))
			return;
	}

	{
		this->_h_wnd = ::CreateWindowEx(
			0,
			wnd_class.lpszClassName,
			cp_window_name,
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			// ウィンドウの横サイズ
			model->Width(),
			// ウィンドウの縦サイズ
			model->Height(),
			NULL,
			NULL,
			h_instance,
			(VOID*)0x12345678
		);
	}

	// Windowのコールバックリストに追加
	{
		PushViewFromCallbackList(this);
	}

	// メッセージループ
	{
		TCHAR t[256] = { 0 };
		do
		{
			if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
			else
			{
				// FPS値をウィンドウのタイトルバーに表示している
				auto start = std::chrono::high_resolution_clock::now();
				{
					// 更新処理
					this->_p_ctrl->Update();

					// 描画開始
					this->_p_ctrl->BeginRender();
					{
						// 描画実行
						this->_p_ctrl->Render();
					}
					// 描画終了
					this->_p_ctrl->EndRender();
				}
				auto end = std::chrono::high_resolution_clock::now();
				auto dur = end - start;
				auto msec = std::chrono::duration_cast<std::chrono::microseconds>(dur).count();
				auto fps = 1000.0f * 1000.0f / (double)msec;
				ZeroMemory(t, sizeof(t));
				_stprintf_s(t, 256, _T("fps: %lld"), (long long)fps);
			}

			SetWindowText(this->_h_wnd, t);
		} while (msg.message != WM_QUIT);
	}
}

GUIWindowWinView::~GUIWindowWinView()
{
	PopViewFromList(this);

	this->_p_ctrl = NULL;
}

// Windowの描画内容書き換えをする
void GUIWindowWinView::WriteRender(std::shared_ptr<GUIWindowModel> in_model)
{
	in_model->ShaderWindowFrameBuffer()->Display(this->_h_wnd, 0, 0);
}

// メインウィンドウイベント処理
LRESULT CALLBACK GUIWindowWinView::MainWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		// ウィンドウ生成時に呼ばれる
	case WM_CREATE:
	{
		::ShowWindow(hWnd, SW_SHOW);
		break;
	}
	// マウスの左ボタンクリック
	case WM_LBUTTONDOWN:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);

		auto view = GUIWindowWinView::SearchViewFromList(hWnd);
		// タッチイベントを呼ぶ
#if _DEBUG
		printf("left button by mouse: x %d / y %d\n", x, y);
#endif
		if (view != NULL)
			view->_p_ctrl->OnTouchEvent(GUIWindowController::eTouchEvent::TOUCH_EVENT_L_CLICK, x, y);

		break;
	}
	case WM_CLOSE:
	{
		::PostMessage(hWnd, WM_QUIT, 0, 0);
		break;
	}
	default:
		break;
	}

	return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void GUIWindowWinView::PushViewFromCallbackList(GUIWindowWinView* in_p_view)
{
	assert(in_p_view != NULL);
	s_window_view_list.push_back(in_p_view);
}

void GUIWindowWinView::PopViewFromList(GUIWindowWinView* in_p_view)
{
	assert(in_p_view != NULL);
	for (auto it = s_window_view_list.begin(); it != s_window_view_list.end(); ++it)
	{
		if (*it == in_p_view)
		{
			s_window_view_list.erase(it);
			break;
		}
	}
}

GUIWindowWinView* GUIWindowWinView::SearchViewFromList(HWND in_h_wnd)
{
	for (auto it = s_window_view_list.begin(); it != s_window_view_list.end(); ++it)
	{
		if ((*it)->_h_wnd == in_h_wnd)
			return *it;
	}

	return NULL;
}