#ifndef __COMMON_COLOR_H__
#define __COMMON_COLOR_H__

#include "utility.h"

namespace ColorUtility
{
	// 0 - 1の値を0 - 255のRGB値に変換
	inline int ConverRGB01ToRGB255(const double a) {
		return static_cast<int>(256 * Clamp(a, 0.0, 0.999));
	}
}

#endif // __COMMON_COLOR_H__
