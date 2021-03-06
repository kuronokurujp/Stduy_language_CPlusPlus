﻿#ifndef __MATH_RAY_H__
#define __MATH_RAY_H__

#include "vec3.h"

class Ray
{
public:
	Ray() {}

	inline void Set(const Color& in_r_origin, const Math::Vec3& in_r_dir)
	{
		this->_orgin.Set(in_r_origin);
		this->_dir.Set(in_r_dir);
		this->_unit_dir.Set(UnitVector3(in_r_dir));
	}

	Point3 At(const double t) const
	{
		return this->_orgin + this->_dir * t;
	}

public:
	Math::Vec3 _orgin;
	Math::Vec3 _dir;
	Math::Vec3 _unit_dir;
};

#endif // __MATH_RAY_H__
