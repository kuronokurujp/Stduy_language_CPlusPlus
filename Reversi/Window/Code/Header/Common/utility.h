#ifndef __COMMON__UITLITY_H__
#define __COMMON__UITLITY_H__

// 共通で使うのを定義

#include <cmath>
#include <cstdlib>
#include <limits>
#include <memory>
#include <random>
#include <algorithm>
#include <assert.h>

// stdの記述を省略
using std::shared_ptr;
using std::make_shared;
using std::sqrt;

// 定数
static const double c_infinity = std::numeric_limits<double>::infinity();
static const double c_pi = 3.1415926535897932385;

// 角度からラジアンへの変換
inline double ConvDegreesToRadians(double in_deg) {
	return in_deg * c_pi / 180.0f;
}

// 最小と最大からの乱数値を整数型で取得
inline int RandomInt(int in_min, int in_max)
{
	assert(in_min < in_max);

	return in_min + rand() % (in_max - in_min + 1);
}

// 0-1からの乱数値を小数点で取得
inline double RandomDouble()
{
	static std::uniform_real_distribution<double> distribution(0.0, 1.0);
	static std::mt19937 generator;
	return distribution(generator);
}

// 最小から最大の乱数値を小数点で取得
inline double RandomDouble(double in_min, double in_max) {
	// min - maxの実数乱数を返す
	return in_min + (in_max - in_min) * RandomDouble();
}

// 最小と最大を指定して最小最大内に収めた値を返す
inline double Clamp(double in_x, double in_min, double in_max)
{
	if (in_x <= in_min)
		return in_min;

	if (in_x >= in_max)
		return in_max;

	return in_x;
}

// 最大値を超えない値を返す
inline double Max(double in_x, double in_max)
{
	if (in_x >= in_max)
		return in_max;

	return in_x;
}

// 共通ヘッダー
#include "Window/Code/Header/Math/ray.h"
#include "Window/Code/Header/Math/vec3.h"

#endif // __COMMON__UITLITY_H__
