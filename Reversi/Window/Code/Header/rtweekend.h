#ifndef __RTWEEKEND_H__
#define __RTWEEKEND_H__

// 共通で使うのを定義

#include <cmath>
#include <cstdlib>
#include <limits>
#include <memory>
#include <random>

// stdの記述を省略
using std::shared_ptr;
using std::make_shared;
using std::sqrt;

// 定数
const double c_infinity = std::numeric_limits<double>::infinity();
const double c_pi = 3.1415926535897932385;

inline double ConvDegreesToRadians(double in_deg)
{
	return in_deg * c_pi / 180.0f;
}

inline double RandomDouble()
{
	static std::uniform_real_distribution<double> distribution(0.0, 1.0);
	static std::mt19937 generator;
	return distribution(generator);
}

inline double RandomDouble(double in_min, double in_max)
{
	// min - maxの実数乱数を返す
	return in_min + (in_max - in_min) * RandomDouble();
}

// 共通ヘッダー
#include "ray.h"
#include "vec3.h"

#endif // __RTWEEKEND_H__
