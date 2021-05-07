#ifndef __PLANE_H__
#define __PLANE_H__

#include "Window/Code/Header/Render/PathTracing/Collision/hit_table.h"
#include "Window/Code/Header/Math/vec3.h"

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
		shared_ptr<Material> in_mat);

	bool Hit(
		const Ray& in_r_ray, double in_t_min, double in_t_max, hit_record& out_r_rec, int in_skip_object_handle
	) const override;

	inline long Handle() const {
		return this->_handle;
	}

	// AABBによる衝突に必要な情報
	bool BoundingBox(double in_t0, double in_t1, AABB& out_box) const override;

public:
	// 長方形の最小座標と最大座標
	// この最大・最小の間を長方形として描画する
	double _x0, _x1, _z0, _z1;
	double _k;
	double _inv_x10, _inv_z10;
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
