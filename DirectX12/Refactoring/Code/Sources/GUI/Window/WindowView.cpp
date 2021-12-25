#include "GUI/GUIMiniHeader.h"

#include <assert.h>
#include <vector>
#include <tchar.h>

namespace GUI
{
    static std::vector<WindowView*> s_window_view_list;

    WindowView::~WindowView()
    {
        if (!PopViewFromList(this))
            return;

        // 生成したウィンドウ登録を解除
        UnregisterClass(this->_wnd_class.lpszClassName, this->_wnd_class.hInstance);
        this->_p_ctrl = NULL;
    }

    void WindowView::CreateObject(WindowControllerInterface* in_p_ctr)
    {
        assert(in_p_ctr != NULL);

        this->_p_ctrl = in_p_ctr;

        auto& window_data = this->_p_ctrl->GetWindowData();

        // アプリのインスタンス
        HINSTANCE h_instance = ::GetModuleHandle(NULL);

        // クラス名称
        const TCHAR* cp_class_name = _T("MainWindowClass");

        // メニュー
        const TCHAR* cp_menu = MAKEINTRESOURCE(NULL);

        // ウィンドウのタイトル名
        const TCHAR* cp_window_name = _T("test");

        // ウィンドウクラスのパラメータ設定
        {
            this->_wnd_class.cbSize = sizeof(WNDCLASSEX);
            this->_wnd_class.style = CS_HREDRAW | CS_VREDRAW;
            this->_wnd_class.lpfnWndProc = MainWindowProc;
            this->_wnd_class.cbClsExtra = 0;
            this->_wnd_class.cbWndExtra = 0;
            this->_wnd_class.hInstance = h_instance;
            this->_wnd_class.hIcon = ::LoadIcon(NULL, IDI_APPLICATION);
            this->_wnd_class.hCursor = ::LoadCursor(NULL, IDC_ARROW);
            this->_wnd_class.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
            this->_wnd_class.lpszMenuName = cp_menu;
            this->_wnd_class.lpszClassName = cp_class_name;
            this->_wnd_class.hIconSm = nullptr;
        }

        // ウィンドウクラス生成
        {
            if (0 == ::RegisterClassEx(&this->_wnd_class))
                return;
        }

        {
            this->_h_wnd = ::CreateWindowEx(
                0,
                this->_wnd_class.lpszClassName,
                cp_window_name,
                WS_OVERLAPPEDWINDOW,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                // ウィンドウの横サイズ
                window_data.width,
                // ウィンドウの縦サイズ
                window_data.height,
                nullptr,
                nullptr,
                h_instance,
                (VOID*)0x12345678
            );
        }

        // Windowのコールバックリストに追加
        PushViewFromCallbackList(this);
    }

    void WindowView::Show()
    {
        ::ShowWindow(this->_h_wnd, SW_SHOW);
    }

    const bool WindowView::PostUpdate()
    {
        if (::PeekMessage(&_msg, NULL, 0, 0, PM_REMOVE))
        {
            ::TranslateMessage(&_msg);
            ::DispatchMessage(&_msg);

            // 終了しているかチェック
            // 終了していたら終了イベントを呼ぶ
            if (this->_msg.message == WM_QUIT)
                this->_p_ctrl->OnAppQuitEvent();

            return false;
        }

        return true;
    }

    // メインウィンドウイベント処理
    LRESULT CALLBACK WindowView::MainWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg)
        {
        case WM_QUIT:
        {
            auto view = WindowView::SearchViewFromList(hWnd);
            if (view != NULL)
                view->_p_ctrl->OnAppQuitEvent();

            break;
        }

        // ウィンドウ生成時に呼ばれる
        case WM_CREATE:
        {
            //            ::ShowWindow(hWnd, SW_SHOW);
            break;
        }
        // マウスの左ボタンクリック
        case WM_LBUTTONDOWN:
        {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);

            auto view = WindowView::SearchViewFromList(hWnd);
            // タッチイベントを呼ぶ
#if _DEBUG
            printf("left button by mouse: x %d / y %d\n", x, y);
#endif
            if (view != NULL)
                view->_p_ctrl->OnTouchEvent(GUI::eTouchEvent::TOUCH_EVENT_L_CLICK, x, y);

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

    void WindowView::PushViewFromCallbackList(WindowView* in_p_view)
    {
        assert(in_p_view != NULL);
        s_window_view_list.push_back(in_p_view);
    }

    const bool WindowView::PopViewFromList(WindowView* in_p_view)
    {
        assert(in_p_view != NULL);
        for (auto it = s_window_view_list.begin(); it != s_window_view_list.end(); ++it)
        {
            if (*it == in_p_view)
            {
                s_window_view_list.erase(it);
                return true;
            }
        }

        return false;
    }

    WindowView* WindowView::SearchViewFromList(HWND in_h_wnd)
    {
        for (auto it = s_window_view_list.begin(); it != s_window_view_list.end(); ++it)
        {
            if ((*it)->_h_wnd == in_h_wnd)
                return *it;
        }

        return NULL;
    }
}
