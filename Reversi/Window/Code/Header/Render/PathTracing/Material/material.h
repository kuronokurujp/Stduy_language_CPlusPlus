#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include "Window/Code/Header/Math/ray.h"

#include "Window/Code/Header/Render/PathTracing/Collision/hit_table.h"
#include "Window/Code/Header/Render/PathTracing/Light/light.h"

// メッシュのマテリアルクラス
class Material {
public:
	virtual ~Material() {}
	virtual bool Scatter(
		const Ray& in_r_ray,
		const hit_record& in_r_hit_recode,
		Color& out_r_attenuation,
		Ray& out_r_scattered,
		LightSpace& in_r_light_space,
		const bool in_b_shadow) const = 0;
};

#endif // __MATERIAL_H__
