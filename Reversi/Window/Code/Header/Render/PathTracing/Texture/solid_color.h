#ifndef __SOLID_COLOR_H__
#define __SOLID_COLOR_H__

#include "texture.h"

// ソリッドカラーのテクスチャ
class SolidColor : public Texture {
public:
	SolidColor() {}
	SolidColor(Color in_r_color) { this->_color_value.Set(in_r_color); }

	Color Value(double in_u, double in_v, const Point3& in_r_p) override {
		return this->_color_value;
	}

private:
	Color _color_value;
};

#endif // __SOLID_COLOR_H__
