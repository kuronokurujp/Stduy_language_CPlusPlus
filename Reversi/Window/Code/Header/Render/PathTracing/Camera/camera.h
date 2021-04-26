#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "Common/utility.h"

class Camera
{
public:
	Camera() {}

	void _Init(Point3 in_lookfrom, Point3 in_lookat, Math::Vec3 in_vup, const double in_fov_degress, const double in_w_size, const double in_h_size)
	{
		auto w = UnitVector3(in_lookfrom - in_lookat);
		auto u = UnitVector3(Cross(in_vup, w));
		auto v = Cross(w, u);

		// 視野角から視野の高さを求める
		auto theta = ConvDegreesToRadians(in_fov_degress);
		auto height = tanh(theta * 0.5f);

		// 画面アスペクトを決める
		this->_aspect_ratio = in_w_size / in_h_size;
		// ビューポート
		// 縦のビューポートサイズは固定
		this->_viewport_height = 2.0 * height;
		// 横のビューポートサイズはアスペクト比に合わせて延ばす
		// つまり横基準の画面
		this->_viewport_width = this->_viewport_height * this->_aspect_ratio;

		// 縦横のビューポートサイズ
		this->_horizontal.Set(this->_viewport_width * u);
		auto half_horizontal = this->_horizontal * 0.5;

		this->_vertical.Set(this->_viewport_height * v);
		auto half_vertical = this->_vertical * 0.5;

		// レイの原点
		this->_origin.Set(in_lookfrom);

		// viewport画面の左下の座標位置
		// (-w / 2, -1.0)となる
		this->_lower_left_cornner.Set(
			this->_origin - (half_vertical)-(half_horizontal)-w);
	}

	void OutputRay(Ray* out_p_ray, double in_u, double in_v) const
	{
		out_p_ray->Set(
			this->_origin,
			this->_lower_left_cornner + this->_horizontal * in_u + this->_vertical * in_v - this->_origin);
	}

public:
	Point3 _origin;
	Point3 _lower_left_cornner;
	Math::Vec3 _horizontal;
	Math::Vec3 _vertical;
	double _aspect_ratio;
	double _viewport_height;
	double _viewport_width;
};

#endif // __CAMERA_H__
