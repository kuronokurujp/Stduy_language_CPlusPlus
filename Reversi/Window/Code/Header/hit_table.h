#ifndef __HIT_TABELE_H__
#define __HIT_TABELE_H__

#include "ray.h"
// shader_ptrなどで使う
#include <memory>

class Material;

struct hit_record
{
	Point3 p;
	Math::Vec3 normal;
	shared_ptr<Material> map_ptr;
	double t;
	// テクスチャーマッピングのUV値
	double u, v;

	bool front_face;
	long object_handle;

	inline void SetFaceNormal(const Ray& in_r_ray, const Math::Vec3& in_outward_normal)
	{
		// 法線はレイの方向と常に逆向きにするようにする
		// レイの方向と法線の方向が鋭角であればレイと法線の方向が同じ向きとなる
		this->front_face = Dot(in_r_ray._unit_dir, in_outward_normal) < 0.0;
		// レイと法線の方向が同じ向きなら法線の方向を逆向きにしてレイと同じ方向しないようにしている　
		// 同方向なら物体の内側からレイが飛んだことになる

		// TODO: なぜそうなるのか納得はいっていない
		this->normal = this->front_face ? in_outward_normal : -in_outward_normal;
	}
};

// インターフェイスクラス
class HitTable
{
public:
	virtual ~HitTable() {}
	virtual bool Hit(
		const Ray& in_r_ray, double in_t_min, double in_t_max, hit_record& out_r_rec, int in_skip_object_handle
	) const = 0;

	virtual inline long Handle() const = 0;
};

#endif // __HIT_TABELE_H__
