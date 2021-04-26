#ifndef __CYLINDER_H__
#define __CYLINDER_H__

#include "Render/PathTracing/Collision/hit_table.h"
#include "Math/vec3.h"

// shader_ptrなどで使う
#include <memory>

#include "combaseapi.h"

class Material;

class Cylinder : public HitTable
{
public:
	Cylinder() { this->_Init(); }
	Cylinder(
		Point3 in_center,
		const Math::Vec3& in_r_axis,
		const double in_radius,
		shared_ptr<Material> in_map_ptr);

	bool Hit(
		const Ray& in_r_ray,
		double in_tmin, double in_tmax,
		hit_record& out_r_rec, int in_skip_object_handle) const override;

	// AABBによる衝突に必要な情報
	bool BoundingBox(double in_t0, double in_t1, AABB& out_box) const override;

	inline long Handle() const override {
		return this->_handle;
	}

	//in_minからin_maxまでにin_xが入ってるかどうか
	static inline bool IsMinMax(double in_x, double in_min, double in_max) {
		return (in_min < in_x && in_x < in_max);
	}

private:
	Point3 _center;
	Point3 _pa;
	Point3 _pb;
	double _radius;

	long _handle;

	shared_ptr<Material> _map_ptr;

private:
	Math::Vec3 _ba;
	double _baba;
	double _inv_baba;
	double _r2baba;

	void _Init()
	{
		// オブジェクトのユニークなIDを生成する
		// オブジェクトの識別に利用
		GUID guid;
		CoCreateGuid(&guid);
		this->_handle = guid.Data1;
	}
};

#endif // __CYLINDER_H__
