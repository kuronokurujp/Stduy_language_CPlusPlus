#include <stdio.h>
#include <locale.h>
#include <tchar.h>
#include <windows.h>
#include <wingdi.h>
#include <chrono>
#include <string>
#include <thread>
#include <mutex>
#include <vector>

#include "GUIWindow/Model/gui_window_model.h"
#include "GUIWindow/View/Win/gui_window_win_view.h"
#include "GUIWindow/gui_window_controller.h"

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

	// レイ照射して取得した色を取得
	void OutputRayColor(Color* in_p_out, const Ray& in_r_ray, const int in_depth, const int in_max_depth)
	{
		if (in_depth <= 0)
			return;

		hit_record rec;
		if (this->bvh_node->Hit(in_r_ray, 0.001, c_infinity, rec, -1))
		{
			// マテリアルによるピクセル色を出力
			auto c = Color();
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

// 描画データモデル
// レイトレース空間から描画データを生成する
class GUITestWindowModel : public GUIWindowModel
{
public:
	// 制御モデルは外部から受け取る
	GUITestWindowModel(
		std::shared_ptr<RayTraceSpace> in_raytrace_space) :
		GUIWindowModel(in_raytrace_space->_width, in_raytrace_space->_height)
	{
		this->_raytrace_space = in_raytrace_space;
		int width = this->_raytrace_space->_width;
		int height = this->_raytrace_space->_height;
		const double d_width = width - 1;
		const double d_height = height - 1;
		this->_inv_d_width = 1.0 / d_width;
		this->_inv_d_heigth = 1.0 / d_height;
	}

	// 描画バッファを更新
	void UpdateRender() override
	{
		// レイトレース空間の情報から画面に表示するピクセルを取得してフレームバッファに書き込む
		{
			auto p_frame_buffer = this->_p_frame_buffer[this->_frame_buffer_count];
			p_frame_buffer->Cls(128);

			int w0 = 0;
			int w1 = 0;
			int h0 = 0;
			int h1 = 0;
			int thread_max = std::thread::hardware_concurrency();
			int width = this->_raytrace_space->_width;
			w1 = width;
			int height = this->_raytrace_space->_height / thread_max;

			std::vector<std::thread> threads;
			for (int i = 0; i < thread_max; ++i) {
				// 書き込むと高さの始めと終わりの位置を計算
				h0 = height * i;
				h1 = height * (i + 1);

				threads.emplace_back(
					[w0, w1, h0, h1, this]() {
					// 読み取りがメインで書き込み先は競合しないのでロックはしない

					_Update(w0, w1, h0, h1 - 1);
				});
			}

			for (auto& t : threads) {
				t.join();
			}
		}

		GUIWindowModel::UpdateRender();
	}

private:
	void _Update(const int in_w0, const int in_w1, const int in_h0, const int in_h1)
	{
		auto p_frame_buffer = this->_p_frame_buffer[this->_frame_buffer_count];
		RayTraceSpace& r_space = *this->_raytrace_space;

		int ir, ig, ib;
		ir = ig = ib = 0;

		double u, v;
		Ray ray;
		Color color;
		Color temp_color;
		// 100位にしないときれいにならない
		// でもそうすると負荷がでかくてFPSが激落ちするのでいったんアンチエイリアスはやらない
		const int sample_per_pixel = 1;
		const int max_depth = 1;

		// 横ラインを先に書き込む
		for (int y = in_h1; y >= in_h0; --y)
		{
			double d_y = double(y);
			v = d_y * this->_inv_d_heigth;

			for (int x = in_w0; x < in_w1; ++x)
			{
				color.Zero();
				temp_color.Zero();
				double d_x = double(x);

				{
					u = d_x * this->_inv_d_width;

					r_space._camera.OutputRay(&ray, u, v);
					r_space.OutputRayColor(&color, ray, max_depth, max_depth);
				}

				{
					ir = ColorUtility::ConverRGB01ToRGB255(color.x());
					ig = ColorUtility::ConverRGB01ToRGB255(color.y());
					ib = ColorUtility::ConverRGB01ToRGB255(color.z());
				}

				p_frame_buffer->SetPixel(x, y, ir, ig, ib);
			}
		}
	}

private:
	std::shared_ptr<RayTraceSpace> _raytrace_space;
	double _inv_d_width;
	double _inv_d_heigth;
};

/// <summary>
/// Mains the specified argc.
/// </summary>
/// <param name="argc">The argc.</param>
/// <param name="argv">The argv.</param>
/// <returns></returns>
int main(int argc, const char * argv[])
{
	setlocale(LC_ALL, "Japanse");

	// レイトレースする空間情報を作成
	// 横の長さが基準なので横のサイズ値を渡す
	auto raytrace_space = make_shared<RayTraceSpace>(640, 16.0, 9.0);
	{
		shared_ptr<CheckerTexture> checkerTexture = make_shared<CheckerTexture>(
			make_shared<SolidColor>(Color(0.0, 1.0, 1.0)),
			make_shared<SolidColor>(Color(0.0, 1.0, 0.0)),
			8);

		auto black = make_shared<SolidColor>(Color(0.0, 0.0, 0.0));
		auto white = make_shared<SolidColor>(Color(1.0, 1.0, 1.0));

		// 版を作成
		raytrace_space->world.Add(make_shared<Plane>(-0.85, 0.85, -0.7, 1.0, 0.0, make_shared<Lambertian>(checkerTexture)));

		// 版に石を載せた
		const int stone_count = 8;
		for (int y = 0; y < stone_count; ++y)
		{
			for (int x = 0; x < stone_count; ++x)
			{
				raytrace_space->world.Add(
					make_shared<Cylinder>(
						Point3(-1.0 + 0.25 + (static_cast<double>(x) * 0.215), 0.0, -1.0 + 0.4 + (static_cast<double>(y) * 0.21)),
						Math::Vec3(0.0, 0.01, 0.0), 0.1,
						make_shared<Lambertian>(black)));
			}
		}

		// オブジェクトをノードツリーで配置してレイ衝突検知を最適化
		raytrace_space->bvh_node = make_shared<BvhNode>(raytrace_space->world, 0, 0);
	}

	std::shared_ptr<GUITestWindowModel> model = std::make_shared<GUITestWindowModel>(raytrace_space);
	std::shared_ptr<GUIWindowWinView> view = std::make_shared<GUIWindowWinView>();

	GUIWindowController ctrl(model, view);
	if (ctrl.Start() == false)
		return -1;

	return 0;
}