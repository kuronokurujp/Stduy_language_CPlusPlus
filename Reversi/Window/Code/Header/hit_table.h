#ifndef __HIT_TABELE_H__
#define __HIT_TABELE_H__

#include "ray.h"

struct hit_record
{
	Point3 p;
	Math::Vec3 normal;
	double t;
	bool front_face;

	inline void SetFaceNormal(const Ray& in_r_ray, const Math::Vec3& in_outward_normal)
	{
		// 法線はレイの方向と常に逆向きにするようにする
		// レイの方向と法線の方向が鋭角であればレイと法線の方向が同じ向きとなる
		this->front_face = Dot(in_r_ray._dir, in_outward_normal) < 0.0;
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
		const Ray& in_r_ray, double in_t_min, double in_t_max, hit_record& out_r_rec
	) const = 0;
};

#endif // __HIT_TABELE_H__
