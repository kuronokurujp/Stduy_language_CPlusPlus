#ifndef __AABB_H__
#define __AABB_H__

#include "Math/vec3.h"
#include "Math/ray.h"

class AABB
{
public:
	AABB() {}
	AABB(const Point3& in_a, const Point3& in_b)
	{
		this->_min.Set(in_a);
		this->_max.Set(in_b);
	}

	inline Point3 Min() const { return this->_min; }
	inline Point3 Max() const { return this->_max; }

	inline bool hit(const Ray& in_r_ray, double in_tmin, double in_tmax) const
	{
		for (int a = 0; a < 3; ++a) {
			auto invD = 1.0f / in_r_ray._dir[a];
			auto t0 = (this->Min()[a] - in_r_ray._orgin[a]) * invD;
			auto t1 = (this->Max()[a] - in_r_ray._orgin[a]) * invD;

			if (invD < 0.0)
				std::swap(t0, t1);

			in_tmin = t0 > in_tmin ? t0 : in_tmin;
			in_tmax = t1 < in_tmax ? t1 : in_tmax;
			if (in_tmax <= in_tmin)
				return false;
		}

		return true;
	}

	static AABB SurroundingBox(AABB in_box0, AABB in_box1)
	{
		Point3 small_pos(
			fmin(in_box0.Min().x(), in_box1.Min().x()),
			fmin(in_box0.Min().y(), in_box1.Min().y()),
			fmin(in_box0.Min().z(), in_box1.Min().z()));

		Point3 big_pos(
			fmax(in_box0.Max().x(), in_box1.Max().x()),
			fmax(in_box0.Max().y(), in_box1.Max().y()),
			fmax(in_box0.Max().z(), in_box1.Max().z()));

		return AABB(small_pos, big_pos);
	}

private:
	Point3 _min;
	Point3 _max;
};

#endif // __AABB_H__
