#ifndef __SPHERE_H__
#define __SPHERE_H__

#include "Window/Code/Header/Render/PathTracing/Collision/hit_table.h"
#include "Window/Code/Header/Math/vec3.h"
// shader_ptrなどで使う
#include <memory>

class Material;

class Sphere : public HitTable
{
public:
	Sphere() { this->_Init(); }
	Sphere(const Point3& in_r_cen, const double in_r, shared_ptr<Material> in_map_ptr)
	{
		this->_Init();
		this->Set(in_r_cen, in_r, in_map_ptr);
	}

	void Set(const Point3& in_cen, const double in_r, shared_ptr<Material> in_map_ptr)
	{
		this->_center.Set(in_cen);
		this->_radius = in_r;
		this->_map_ptr = in_map_ptr;
	}

	bool Hit(
		const Ray& in_r_ray, double in_tmin, double in_tmax, hit_record& in_r_rec, int in_skip_object_handle
	) const override;

	inline long Handle() const override { return this->_handle; };

	// AABBによる衝突に必要な情報
	bool BoundingBox(double in_t0, double in_t1, AABB& out_box) const override;

public:
	Point3 _center;
	double _radius;
	long _handle;
	shared_ptr<Material> _map_ptr;

private:
	void _Init();
	static void _OutputUV(const Math::Vec3& in_r_p, double* in_p_u, double* in_p_v);
};

#endif // __SPHERE_H__
