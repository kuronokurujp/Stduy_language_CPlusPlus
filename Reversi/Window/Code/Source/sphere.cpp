#include "sphere.h"

bool Sphere::Hit(
	const Ray & in_r_ray, double in_tmin, double in_tmax, hit_record & out_r_rec) const
{
	// レイとと球の衝突判定を2次方程式の判別式で行っている

	// 球の座標からレイの原点へと向かうベクトル
	Math::Vec3 oc = in_r_ray._orgin - this->_center;

	// レイのベクトル長さの2乗
	auto a = in_r_ray._dir.LengthSquared();

	// 球とレイの原点のベクトルとレイベクトルの内積
	auto half_b = Dot(oc, in_r_ray._dir);

	// 球の方程式(x * x + y * y + z * z - (r * r) = 0)
	auto c = oc.LengthSquared() - this->_radius * this->_radius;

	// 2次方程式の判別式から接触しているか判定
	auto discrimimant = half_b * half_b - a * c;
	if (discrimimant <= 0.0)
		return false;

	auto root = sqrt(discrimimant);
	auto temp = (-half_b - root) / a;
	if (temp < in_tmax && temp > in_tmin)
	{
		out_r_rec.t = temp;
		out_r_rec.p = in_r_ray.At(out_r_rec.t);
		Math::Vec3 outward_normal = (out_r_rec.p - this->_center) / this->_radius;
		out_r_rec.SetFaceNormal(in_r_ray, outward_normal);

		return true;
	}

	temp = (-half_b + root) / a;
	if (temp < in_tmax && temp > in_tmin)
	{
		out_r_rec.t = temp;
		out_r_rec.p = in_r_ray.At(out_r_rec.t);

		Math::Vec3 outward_normal = (out_r_rec.p - this->_center) / this->_radius;
		out_r_rec.SetFaceNormal(in_r_ray, outward_normal);

		return true;
	}

	return false;
}