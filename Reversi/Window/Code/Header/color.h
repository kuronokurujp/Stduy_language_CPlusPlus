#ifndef __COLOR_H__
#define __COLOR_H__

#include "vec3.h"
#include "rtweekend.h"

namespace ColorUtility
{
	inline int ConverRGB01ToRGB255(const double a)
	{
		return static_cast<int>(256 * Clamp(a, 0.0, 0.999));
	}
}

#endif // __COLOR_H__
