#ifndef __COMMON_COLOR_H__
#define __COMMON_COLOR_H__

#include "rtweekend.h"

namespace ColorUtility
{
	inline int ConverRGB01ToRGB255(const double a) {
		return static_cast<int>(256 * Clamp(a, 0.0, 0.999));
	}
}

#endif // __COMMON_COLOR_H__
