#ifndef __RAY_H__
#define __RAY_H__

#include "vec3.h"

class Ray
{
public:
	Ray() {}

	inline void Set(const Point3& in_r_origin, const Math::Vec3& in_r_dir)
	{
		this->_orgin.Set(in_r_origin);
		this->_dir.Set(in_r_dir);
	}

	Point3 At(const double t) const
	{
		return this->_orgin + this->_dir * t;
	}

public:
	Math::Vec3 _orgin;
	Math::Vec3 _dir;
};

void OutputRayColor(Point3* in_p_out, Ray& in_r_ray)
{
	Math::Vec3 unit_direction = UnitVector3(in_r_ray._dir);
	auto t = 0.5 * (unit_direction.y() + 1.0);
	*in_p_out = (1.0 - t) * Color(1.0, 1.0, 1.0) + t * Color(0.5, 0.7, 1.0);
}

#endif // __RAY_H__
