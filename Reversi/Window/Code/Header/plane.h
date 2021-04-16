#ifndef __PLANE_H__
#define __PLANE_H__

#include "hit_table.h"
#include "vec3.h"

// shader_ptrなどで使う
#include <memory>

#include "combaseapi.h"

class Material;

class Plane : public HitTable
{
public:
	Plane() { this->_Init(); }
	Plane(
		// 左下の座標
		const double in_x0, const double in_x1,
		// 右上の座標
		const double in_z0, const double in_z1,
		// 高さになる
		const double in_k,
		shared_ptr<Material> in_mat) {
		this->_Init();

		this->_x0 = in_x0;
		this->_x1 = in_x1;
		this->_z0 = in_z0;
		this->_z1 = in_z1;

		this->_k = in_k;
		this->_map_ptr = in_mat;
	}

	bool Hit(
		const Ray& in_r_ray, double in_t_min, double in_t_max, hit_record& out_r_rec, int in_skip_object_handle
	) const override
	{
		// 光線から接触位置を取得
		auto t = (this->_k - in_r_ray._orgin.y()) / in_r_ray._dir.y();
		// 接触位置が範囲外であれば接触していない
		if (t < in_t_min || t > in_t_max)
			return false;

		// 接触しても長方形内に接触していないなら接触していない
		auto x = in_r_ray._orgin.x() + t * in_r_ray._dir.x();
		auto z = in_r_ray._orgin.z() + t * in_r_ray._dir.z();
		if (x < this->_x0 || x > this->_x1 || z < this->_z0 || z > this->_z1)
			return false;

		out_r_rec.u = (x - this->_x0) / (this->_x1 - this->_x0);
		out_r_rec.v = (z - this->_z0) / (this->_z1 - this->_z0);
		out_r_rec.t = t;

		auto outward_normal = Math::Vec3(0, 1, 0);
		out_r_rec.SetFaceNormal(in_r_ray, outward_normal);
		out_r_rec.map_ptr = this->_map_ptr;
		out_r_rec.p = in_r_ray.At(t);
		out_r_rec.object_handle = this->_handle;

		return true;
	}

	long Handle() const
	{
		return this->_handle;
	}

public:
	// 長方形の最小座標と最大座標
	// この最大・最小の間を長方形として描画する
	double _x0, _x1, _z0, _z1;
	double _k;
	shared_ptr<Material> _map_ptr;

	long _handle;

private:
	void _Init()
	{
		// オブジェクトのユニークなIDを生成する
		// オブジェクトの識別に利用
		GUID guid;
		CoCreateGuid(&guid);
		this->_handle = guid.Data1;
	}
};

#endif // __PLANE_H__
