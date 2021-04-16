#ifndef __MATERIAL_LAMBERTIAN_H__
#define __MATERIAL_LAMBERTIAN_H__

#include "material.h"
#include "light.h"
#include "texture.h"

// ランバート拡散モデルのマテリアル
class Lambertian : public Material
{
public:
	Lambertian(shared_ptr<Texture> in_texture) {
		this->_albedo = in_texture;
	}

	bool Scatter(
		const Ray& in_r_ray,
		const hit_record& in_r_hit_recode,
		Color& out_r_attenuation,
		Ray& out_r_scattered,
		LightSpace& in_r_light_space,
		const bool in_b_shadow) const override
	{
		if (in_b_shadow) {
			out_r_attenuation.Set(0, 0, 0);
			return true;
		}

		// 物体とライトの照射方向とのマテリアル計算
		{
			auto L_Revers = -in_r_light_space._dir;
			auto kd = 1.0;
			auto V = in_r_ray._unit_dir;
			auto N = UnitVector3(in_r_hit_recode.normal);
			// 拡散値
			auto d = Dot(N, L_Revers);

			auto albedo = this->_albedo->Value(in_r_hit_recode.u, in_r_hit_recode.v, in_r_hit_recode.p);

			// アンビエントカラーと乗算したのを基準色とする
			albedo.Set(albedo * in_r_light_space._ambient_color);

			out_r_attenuation = kd * Clamp(d, 0.0, 1.0) * albedo;
		}

		return true;
	}

public:
	shared_ptr<Texture> _albedo;
};

#endif // __MATERIAL_LAMBERTIAN_H__
