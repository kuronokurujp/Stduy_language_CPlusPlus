#ifndef __SPHERE_H__
#define __SPHERE_H__

#include "hit_table.h"
#include "vec3.h"

class Sphere : public HitTable
{
public:
	Sphere() {}
	Sphere(const Point3& in_r_cen, const double in_r)
	{
		this->Set(in_r_cen, in_r);
	}

	void Set(const Point3& in_cen, const double in_r)
	{
		this->_center.Set(in_cen);
		this->_radius = in_r;
	}

	bool Hit(
		const Ray& in_r_ray, double in_tmin, double in_tmax, hit_record& in_r_rec
	) const override;

public:
	Point3 _center;
	double _radius;
};

#endif // __SPHERE_H__
