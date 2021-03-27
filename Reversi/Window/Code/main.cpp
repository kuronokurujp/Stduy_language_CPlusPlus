﻿#include <stdio.h>
#include <locale.h>
#include <tchar.h>
#include <windows.h>
#include <wingdi.h>
#include <chrono>
#include <string>

#include "window_frame_buffer.h"

#include "rtweekend.h"
#include "color.h"
#include "hit_table_list.h"
#include "sphere.h"
#include "camera.h"

// レイトレースを行う空間
class RayTraceSpace
{
public:
	RayTraceSpace(const int in_screen_size, const double in_w_size, const double in_h_size)
	{
		this->_camera.Init(in_w_size, in_h_size);

		// ウィンドウの縦横サイズ
		this->_width = in_screen_size;
		this->_height = static_cast<int>(static_cast<double>(this->_width) / this->_camera._aspect_ratio);

		this->_screen_upper_corrner_color.Set(1.0, 1.0, 1.0);
		this->_screen_under_corrner_color.Set(0.5, 0.7, 1.0);
	}

	// レイ照射して取得した色を取得
	void OutputRayColor(Color* in_p_out, const Ray& in_r_ray)
	{
		hit_record rec;
		if (this->world.Hit(in_r_ray, 0, c_infinity, rec))
		{
			*in_p_out = 0.5 * (rec.normal + Color(1, 1, 1));
			return;
		}

		const Math::Vec3& dir = in_r_ray._dir;
		Math::Vec3 unit_direction = UnitVector3(dir);
		// yは-1 < y < 1となる
		// y + 1.0 => 0 < y < 2
		// (y + 1.0) * 0.5 => 0 < y < 1
		// となり0 から 1の値に変換
		auto t = 0.5 * (unit_direction.y() + 1.0);
		// 線形補間で色を決めている
		// 画面上がt=1で下がt=0
		*in_p_out = (1.0 - t) * this->_screen_upper_corrner_color + t * this->_screen_under_corrner_color;
	}

public:

	// ウィンドウを作る
	int _width;
	int _height;

	Color _screen_upper_corrner_color;
	Color _screen_under_corrner_color;

	HitTableList world;
	Camera _camera;
};

LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// 更新処理
static void Update(FrameBuffer& in_r_frame_buffer, RayTraceSpace& in_r_screen_space);

/// <summary>
/// Mains the specified argc.
/// </summary>
/// <param name="argc">The argc.</param>
/// <param name="argv">The argv.</param>
/// <returns></returns>
int main(int argc, const char * argv[])
{
	setlocale(LC_ALL, "Japanse");

	HWND h_wnd;
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
			return -1;
	}

	// レイトレースする空間情報を作成
	// 横の長さを基準なので横のサイズを値を渡す
	auto raytrace_space = RayTraceSpace(480, 16.0, 9.0);
	{
		raytrace_space.world.Add(make_shared<Sphere>(Point3(0, 0, -1.0), 0.5));
		raytrace_space.world.Add(make_shared<Sphere>(Point3(0, -100.5, -1.0), 100));
	}

	{
		h_wnd = ::CreateWindowEx(
			0,
			wnd_class.lpszClassName,
			cp_window_name,
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			// ウィンドウの横サイズ
			raytrace_space._width,
			// ウィンドウの縦サイズ
			raytrace_space._height,
			NULL,
			NULL,
			h_instance,
			(VOID*)0x12345678
		);
	}

	// ウィンドウに転送する画像情報を転送するバッファ
	WindowFrameBuffer window_frame_buffer(raytrace_space._width, raytrace_space._height);

	// アプリ側が書き込む画像情報
	// ダブルバッファを用意
	FrameBuffer* p_frame_buffer[2] = {
		new FrameBuffer(raytrace_space._width, raytrace_space._height),
		new FrameBuffer(raytrace_space._width, raytrace_space._height)
	};
	int frame_buffer_count = 0;

	// メッセージループ
	{
		TCHAR t[256] = { 0 };
		do
		{
			Sleep(1);

			// FPS値をウィンドウのタイトルバーに表示している
			auto start = std::chrono::high_resolution_clock::now();
			{
				if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
				{
					::TranslateMessage(&msg);
					::DispatchMessage(&msg);
				}
				else
				{
					// 更新
					Update(*(p_frame_buffer[frame_buffer_count]), raytrace_space);

					// カレントフレームバッファで書き込む
					window_frame_buffer.Flash(p_frame_buffer[frame_buffer_count]);
					// バッファを切り替え
					frame_buffer_count = (frame_buffer_count + 1) % 2;

					// フレームバッファで書き込んだ情報を元に転送
					window_frame_buffer.Display(h_wnd, 0, 0);
				}
			}
			auto end = std::chrono::high_resolution_clock::now();
			auto dur = end - start;
			auto msec = std::chrono::duration_cast<std::chrono::microseconds>(dur).count();
			auto fps = 1000.0f * 1000.0f / (double)msec;
			ZeroMemory(t, sizeof(t));
			_stprintf_s(t, 256, _T("fps: %lld"), (long long)fps);
			SetWindowText(h_wnd, t);
		} while (msg.message != WM_QUIT);
	}

	for (int i = 0; i < 2; ++i)
	{
		delete p_frame_buffer[i];
		p_frame_buffer[i] = NULL;
	}
	window_frame_buffer.Free();

	return msg.wParam;
}

// メインウィンドウイベント処理
LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		// ウィンドウ生成時に呼ばれる
	case WM_CREATE:
	{
		::ShowWindow(hWnd, SW_SHOW);
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

void Update(FrameBuffer& in_r_frame_buffer, RayTraceSpace& in_r_space)
{
	in_r_frame_buffer.Cls(128);

	int width = in_r_space._width;
	int height = in_r_space._height;
	int ir, ig, ib = 0;
	const double d_width = width - 1;
	const double d_height = height - 1;
	const double inv_d_width = 1.0 / d_width;
	const double inv_d_heigth = 1.0 / d_height;

	double u, v;
	Ray ray;
	Color color;

	// 横ラインを先に書き込む
	for (int y = height - 1; y >= 0; --y)
	{
		for (int x = 0; x < width; ++x)
		{
			u = double(x) * inv_d_width;
			v = double(y) * inv_d_heigth;

			in_r_space._camera.OutputRay(&ray, u, v);
			in_r_space.OutputRayColor(&color, ray);

			ir = ColorUtility::ConverRGB01ToRGB255(color.x());
			ig = ColorUtility::ConverRGB01ToRGB255(color.y());
			ib = ColorUtility::ConverRGB01ToRGB255(color.z());

			in_r_frame_buffer.SetPixel(x, y, ir, ig, ib);
		}
	}
}