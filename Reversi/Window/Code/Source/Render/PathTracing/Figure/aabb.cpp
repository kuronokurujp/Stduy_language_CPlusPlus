#include "Window/Code/Header/Render/PathTracing/Figure/aabb.h"

AABB::AABB(const Point3& in_a, const Point3& in_b)
{
	this->_min.Set(in_a);
	this->_max.Set(in_b);
}

AABB AABB::SurroundingBox(AABB in_box0, AABB in_box1)
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