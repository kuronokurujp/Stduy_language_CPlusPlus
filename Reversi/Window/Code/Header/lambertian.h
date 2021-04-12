#ifndef __MATERIAL_LAMBERTIAN_H__
#define __MATERIAL_LAMBERTIAN_H__

#include "material.h"
#include "light.h"

// ランバート拡散モデルのマテリアル
class Lambertian : public Material {
public:
	Lambertian(const Color& in_r_color) {
		this->_albedo.Set(in_r_color);
	}

	bool Scatter(
		const Ray& in_r_ray,
		const hit_record& in_r_hit_recode,
		Color& out_r_attenuation,
		Ray& out_r_scattered,
		LightSpace& in_r_light_space,
		const bool in_b_shadow) const override {
		if (in_b_shadow) {
			out_r_attenuation.Set(0, 0, 0);
			return true;
		}

		// 物体とライトの照射方向とのマテリアル計算
		auto L = in_r_hit_recode.p - in_r_light_space._pos;
		L = UnitVector3(L);

		auto s = 1.0;

		Point3 R;
		{
			auto L_Revers = -L;
			auto kd = 0.7, ks = 0.7, ke = 0.3;
			auto V = in_r_ray._unit_dir;
			auto N = in_r_hit_recode.normal;
			// 拡散値
			auto d = Dot(N, L_Revers);

			// 反射ベクトル
			R.Set(L_Revers - (2.0 * d * N));
			// 視線と反射ベクトルが近い程に反射値が増える
			auto rv = Dot(-R, V);
			auto cc = ks * pow(Max(rv, 0), 20) * s * in_r_light_space._ambient_color;

			out_r_attenuation = kd * Max(d, 0.0) * s * this->_albedo + cc + ke * this->_albedo;
		}

		return true;
	}

public:
	Color _albedo;
};

#endif // __MATERIAL_LAMBERTIAN_H__
