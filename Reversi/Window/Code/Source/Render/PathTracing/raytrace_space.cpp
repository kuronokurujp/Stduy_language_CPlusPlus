#include "Render/PathTracing/raytrace_space.h"

#include "Render/PathTracing/Material/material.h"
#include "Render/PathTracing/Collision/hit_table.h"

RayTraceSpace::RayTraceSpace(const int in_screen_size, const double in_w_size, const double in_h_size) :
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
void RayTraceSpace::OutputRayColor(Color* in_p_out, const Ray& in_r_ray, const int in_depth, const int in_max_depth)
{
	if (in_depth <= 0)
		return;

	if (this->bvh_node)
	{
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