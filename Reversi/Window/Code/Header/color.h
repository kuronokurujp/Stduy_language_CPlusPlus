#ifndef __COLOR_H__
#define __COLOR_H__

#include "vec3.h"

namespace ColorUtility
{
	inline int ConverRGB01ToRGB255(const double a)
	{
		return static_cast<int>(255.999 * a);
	}
}

#endif // __COLOR_H__
