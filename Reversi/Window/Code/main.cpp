#include <stdio.h>
#include <locale.h>
#include <tchar.h>
#include <windows.h>
#include <wingdi.h>
#include <chrono>
#include <string>

#include "GUIWindow/Win/window_frame_buffer.h"

#include "Common/rtweekend.h"
#include "Common/color.h"

#include "Math/vec3.h"
#include "Math/ray.h"

#include "Render/PathTracing/Collision/hit_table_list.h"

#include "Render/PathTracing/Figure/sphere.h"
#include "Render/PathTracing/Figure/plane.h"
#include "Render/PathTracing/Figure/cylinder.h"
#include "Render/PathTracing/Figure/aabb.h"

#include "Render/PathTracing/Camera/camera.h"

#include "Render/PathTracing/Material/material.h"
#include "Render/PathTracing/Material/lambertian.h"
#include "Render/PathTracing/Material/metal.h"

#include "Render/PathTracing/Light/light.h"

#include "Render/PathTracing/Collision/bvh_node.h"

// テクスチャー
#include "Render/PathTracing/Texture/solid_color.h"
#include "Render/PathTracing/Texture/checker_texture.h"

// パストレースの幾何学計算は以下のサイトにまとまっている
// 神様のページ
// https://iquilezles.org/www/articles/intersectors/intersectors.htm

// レイトレースを行う空間
class RayTraceSpace
{
public:
	RayTraceSpace(const int in_screen_size, const double in_w_size, const double in_h_size) :
		// ライトスペース情報を設定
		_light_space(Math::Vec3(0.0, -1.0, 0.0), Color(1.0, 1.0, 1.0))
	{
		this->_camera._Init(Point3(0.0, 1.5, 1.0), Point3(0.0, -1.0, 0.0), Math::Vec3(0.0, 1.0, 0.0), 90.0, in_w_size, in_h_size);

		// ウィンドウの縦横サイズ
		this->_width = in_screen_size;
		this->_height = static_cast<int>(static_cast<double>(this->_width) / this->_camera._aspect_ratio);

		this->_screen_upper_corrner_color.Set(1.0, 1.0, 1.0);
		this->_screen_under_corrner_color.Set(0.5, 0.7, 1.0);
	}

	void OutputScreenColor(int* out_p_r, int* out_p_g, int* out_p_b, Color& in_r_color, int in_sample_per_pixel)
	{
		auto r = in_r_color.x();
		auto g = in_r_color.y();
		auto b = in_r_color.z();

		if (in_sample_per_pixel <= 1)
		{
			*out_p_r = ColorUtility::ConverRGB01ToRGB255(r);
			*out_p_g = ColorUtility::ConverRGB01ToRGB255(g);
			*out_p_b = ColorUtility::ConverRGB01ToRGB255(b);
			return;
		}

		double scale = 1.0 / in_sample_per_pixel;
		r = r * scale;
		g = g * scale;
		b = b * scale;

		*out_p_r = ColorUtility::ConverRGB01ToRGB255(r);
		*out_p_g = ColorUtility::ConverRGB01ToRGB255(g);
		*out_p_b = ColorUtility::ConverRGB01ToRGB255(b);
	}

	// レイ照射して取得した色を取得
	void OutputRayColor(Color* in_p_out, const Ray& in_r_ray, const int in_depth, const int in_max_depth)
	{
		if (in_depth <= 0)
			return;

		hit_record rec;
		//if (this->world.Hit(in_r_ray, 0.001, c_infinity, rec, -1))
		if (this->bvh_node->Hit(in_r_ray, 0.001, c_infinity, rec, -1))
		{
			// マテリアルによるピクセル色を出力
			auto c = Color(0.0, 0.0, 0.0);
			{
				Ray chk_ray;
				rec.map_ptr->Scatter(in_r_ray, rec, c, chk_ray, this->_light_space, false);
			}

			*in_p_out += c;
			return;
		}

		// yは-1 < y < 1となる
		// y + 1.0 => 0 < y < 2
		// (y + 1.0) * 0.5 => 0 < y < 1
		// となり0 から 1の値に変換
		auto t = 0.5 * (in_r_ray._unit_dir.y() + 1.0);
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
	shared_ptr<BvhNode> bvh_node;
	Camera _camera;
	LightSpace _light_space;
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
	// 横の長さが基準なので横のサイズ値を渡す
	auto raytrace_space = RayTraceSpace(640, 16.0, 9.0);
	{
		shared_ptr<CheckerTexture> checkerTexture = make_shared<CheckerTexture>(
			make_shared<SolidColor>(Color(0.0, 1.0, 1.0)),
			make_shared<SolidColor>(Color(0.0, 1.0, 0.0)),
			8);

		auto black = make_shared<SolidColor>(Color(0.0, 0.0, 0.0));
		auto white = make_shared<SolidColor>(Color(1.0, 1.0, 1.0));

		// 版を作成
		raytrace_space.world.Add(make_shared<Plane>(-0.85, 0.85, -0.7, 1.0, 0.0, make_shared<Lambertian>(checkerTexture)));

		// 版に石を載せた
		const int stone_count = 8;
		for (int y = 0; y < stone_count; ++y)
		{
			for (int x = 0; x < stone_count; ++x)
			{
				raytrace_space.world.Add(
					make_shared<Cylinder>(
						Point3(-1.0 + 0.25 + (static_cast<double>(x) * 0.215), 0.0, -1.0 + 0.4 + (static_cast<double>(y) * 0.21)),
						Math::Vec3(0.0, 0.01, 0.0), 0.1,
						make_shared<Lambertian>(black)));
			}
		}

		// オブジェクトをノードツリーで配置してレイ衝突検知を最適化
		raytrace_space.bvh_node = make_shared<BvhNode>(raytrace_space.world, 0, 0);
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
	Color temp_color;
	// 100位にしないときれいにならない
	// でもそうすると負荷がでかくてFPSが激落ちするのでいったんアンチエイリアスはやらない
	const int sample_per_pixel = 1;
	const int max_depth = 1;

	// 横ラインを先に書き込む
	for (int y = height - 1; y >= 0; --y)
	{
		double d_y = double(y);
		v = d_y * inv_d_heigth;

		for (int x = 0; x < width; ++x)
		{
			color.Set(0.0, 0.0, 0.0);
			temp_color.Set(0.0, 0.0, 0.0);
			double d_x = double(x);
			/*
			if (sample_per_pixel > 1)
			{
				for (int s = 0; s < sample_per_pixel; ++s)
				{
					u = (d_x + RandomDouble()) * inv_d_width;
					v = (double(y) + RandomDouble()) * inv_d_heigth;

					in_r_space._camera.OutputRay(&ray, u, v);
					in_r_space.OutputRayColor(&temp_color, ray, max_depth, max_depth);
					color += temp_color;
				}
			}
			else
			*/
			{
				u = d_x * inv_d_width;

				in_r_space._camera.OutputRay(&ray, u, v);
				in_r_space.OutputRayColor(&color, ray, max_depth, max_depth);
			}

			in_r_space.OutputScreenColor(&ir, &ig, &ib, color, sample_per_pixel);
			in_r_frame_buffer.SetPixel(x, y, ir, ig, ib);
		}
	}
}