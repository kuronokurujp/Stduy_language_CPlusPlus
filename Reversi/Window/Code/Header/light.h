#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "vec3.h"

class LightSpace
{
public:
	LightSpace(const Math::Vec3& in_r_dir, const Color& in_r_color)
	{
		this->_dir.Set(UnitVector3(in_r_dir));
		this->_ambient_color.Set(in_r_color);
	}

	Math::Vec3 _dir;
	Color _ambient_color;
};

#endif // __LIGHT_H__
