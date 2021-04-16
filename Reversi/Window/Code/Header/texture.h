#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "color.h"
#include "vec3.h"

// テクスチャのインターフェイス
class Texture {
public:
	Texture() {}
	virtual Color Value(double in_u, double in_v, const Point3& in_r_p) = 0;
};

#endif // __TEXTURE_H__
