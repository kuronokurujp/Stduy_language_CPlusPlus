#ifndef __CHECKER_TEXTURE_H__
#define __CHECKER_TEXTURE_H__

#include "texture.h"

class CheckerTexture : public Texture {
public:
	CheckerTexture(shared_ptr<Texture> in_t0, shared_ptr<Texture> in_t1, const int in_masu_number) :
		_even(in_t0),
		_odd(in_t1)
	{
		this->_masu_number = static_cast<double>(in_masu_number);
	}

	Color Value(double in_u, double in_v, const Point3& in_r_p) override {
		if (static_cast<int>(in_u * this->_masu_number) % 2 == 0) {
			if (static_cast<int>(in_v * this->_masu_number) % 2 == 0)
				return this->_even->Value(in_u, in_v, in_r_p);
			else
				return this->_odd->Value(in_u, in_v, in_r_p);
		}
		else {
			if (static_cast<int>(in_v * this->_masu_number) % 2 == 0)
				return this->_odd->Value(in_u, in_v, in_r_p);
			else
				return this->_even->Value(in_u, in_v, in_r_p);
		}
	}

private:
	shared_ptr<Texture> _even;
	shared_ptr<Texture> _odd;

	double _masu_number;
};

#endif // __CHECKER_TEXTURE_H__
