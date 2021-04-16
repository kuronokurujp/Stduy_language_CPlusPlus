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

	long Handle() const override { return -1; };

public:
	std::vector<shared_ptr<HitTable>> _objects;
};

#endif // __HITTABLE_LIST_H__
