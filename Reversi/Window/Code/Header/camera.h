#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "rtweekend.h"

class Camera
{
public:
	Camera() {}

	void Init(const double in_w_size, const double in_h_size)
	{
		// 画面アスペクトを決める
		this->_aspect_ratio = in_w_size / in_h_size;
		// ビューポート
		// 縦のビューポートサイズは固定
		this->_viewport_height = 2.0;
		// 横のビューポートサイズはアスペクト比に合わせて延ばす
		// つまり横基準の画面
		this->_viewport_width = this->_viewport_height * this->_aspect_ratio;

		// 縦横のビューポートサイズ
		this->_horizontal.Set(this->_viewport_width, 0.0, 0.0);
		auto half_horizontal = this->_horizontal * 0.5;

		this->_vertical.Set(0.0, this->_viewport_height, 0.0);
		auto half_vertical = this->_vertical * 0.5;

		// レイの視線の長さ
		auto focal_length = 1.0;

		// レイの原点
		this->_origin = Color(0.0, 0.0, 0.0);

		// レイの視線先を作成
		this->_lower_left_cornner.Set(
			this->_origin - (half_vertical)-(half_horizontal)-Math::Vec3(0.0, 0.0, focal_length));
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
