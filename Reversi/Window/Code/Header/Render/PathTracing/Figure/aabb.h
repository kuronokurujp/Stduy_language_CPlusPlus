#ifndef __AABB_H__
#define __AABB_H__

#include "Window/Code/Header/Math/ray.h"

class AABB
{
public:
	AABB() {}
	AABB(const Point3& in_a, const Point3& in_b);

	inline Point3 Min() const { return this->_min; }
	inline Point3 Max() const { return this->_max; }

	bool hit(const Ray& in_r_ray, double in_tmin, double in_tmax) const
	{
		for (int a = 0; a < 3; ++a) {
			auto invD = 1.0f / in_r_ray._dir[a];
			auto orgin_value = in_r_ray._orgin[a];

			auto t0 = this->Min()[a] - orgin_value;
			auto t1 = this->Max()[a] - orgin_value;

			t0 *= invD;
			t1 *= invD;

			if (invD < 0.0)
				std::swap(t0, t1);

			in_tmin = t0 > in_tmin ? t0 : in_tmin;
			in_tmax = t1 < in_tmax ? t1 : in_tmax;
			if (in_tmax <= in_tmin)
				return false;
		}

		return true;
	}

	static AABB SurroundingBox(AABB in_box0, AABB in_box1);

private:
	Point3 _min;
	Point3 _max;
};

#endif // __AABB_H__
