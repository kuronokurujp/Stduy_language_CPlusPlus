#include "Window/Code/Header/Render/PathTracing/Collision/hit_table_list.h"

bool HitTableList::Hit(
	const Ray& in_r_ray, double in_t_min, double in_t_max, hit_record& out_r_rec, int in_skip_object_handle
) const
{
	// in_t_maxは初期値は無限値とする
	// その後オブジェクトにヒットしたらヒットしたオブジェクトからin_t_maxを取得して更新
	// こうする事でオブジェクトの奥行を考えた結果になる

	hit_record temp_rec;
	bool hit_anything = false;
	auto closest_so_far = in_t_max;
	for (const auto& object : this->_objects)
	{
		auto handle = object->Handle();
		if (object->Hit(in_r_ray, in_t_min, closest_so_far, temp_rec, in_skip_object_handle))
		{
			// オブジェクトにレイがヒット
			// レイのヒット範囲を更新
			// こうする事で奥のオブジェクトにヒットする事がなくなる
			if (handle == in_skip_object_handle)
				hit_anything = false;
			else
				hit_anything = true;

			closest_so_far = temp_rec.t;
			out_r_rec = temp_rec;
		}
	}

	return hit_anything;
}