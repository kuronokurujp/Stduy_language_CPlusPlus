#ifndef __HITTABLE_LIST_H__
#define __HITTABLE_LIST_H__

#include "hit_table.h"

// shader_ptrなどで使う
#include <memory>
// listなどで使う
#include <vector>

using std::shared_ptr;
using std::make_shared;

class HitTableList : public HitTable
{
public:
	HitTableList() {}
	HitTableList(shared_ptr<HitTable> in_object) {
		this->Add(in_object);
	}

	void Clear() {
		this->_objects.clear();
	}

	void Add(shared_ptr<HitTable> in_object) {
		this->_objects.push_back(in_object);
	}

	bool Hit(
		const Ray& in_r_ray, double in_t_min, double in_t_max, hit_record& out_r_rec, int in_skip_object_handle
	) const override;

	inline long Handle() const override { return -1; };

	// AABBによる衝突に必要な情報
	bool BoundingBox(double in_t0, double in_t1, AABB& out_box) const override
	{
		if (this->_objects.empty())
			return false;

		AABB temp_box;
		bool first_box = true;
		for (const auto& object : this->_objects)
		{
			// オブジェクトが持っているAABBの情報を取得
			if (!object->BoundingBox(in_t0, in_t1, temp_box))
				return false;

			out_box = first_box ? temp_box : AABB::SurroundingBox(out_box, temp_box);
			first_box = false;
		}

		return true;
	}

public:
	std::vector<shared_ptr<HitTable>> _objects;
};

#endif // __HITTABLE_LIST_H__
