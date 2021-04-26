#ifndef __BVH_NODE_H__
#define __BVH_NODE_H__

#include "Common/utility.h"
#include "hit_table.h"
#include "hit_table_list.h"
#include "Render/PathTracing/Figure/aabb.h"

#include <memory>

class BvhNode : public HitTable {
public:
	BvhNode() {
	}

	BvhNode(HitTableList& in_r_list, double in_time0, double in_time1)
		: BvhNode(in_r_list._objects, 0, in_r_list._objects.size(), in_time0, in_time1) {}

	BvhNode(
		std::vector<shared_ptr<HitTable>>& in_r_objects,
		size_t in_start, size_t in_end, double in_time0, double in_time1)
	{
		// xyzの３つの内どれか一つの軸をランダムで選択
		int axis = RandomInt(0, 2);

		auto comparator = BoxXCompare;
		if (axis == 1)
			comparator = BoxYCompare;
		else if (axis == 2)
			comparator = BoxZCompare;

		size_t object_span = in_end - in_start;
		// 絞り込む場合
		// 最終的にobject_spanは1 or 2になり、左右ノードにAABBのオブジェクトが設定される
		// その場合右より左のオブジェクトの方が位置として手前となる
		// 絞り込んだノードが１つの場合
		if (object_span == 1) {
			this->_left = this->_right = in_r_objects[in_start];
		}
		else if (object_span == 2) {
			// 手前のAABBがどちらか判別して
			// 手前のAABBを左ノードに設定するようにする
			if (comparator(in_r_objects[in_start], in_r_objects[in_start + 1])) {
				this->_left = in_r_objects[in_start];
				this->_right = in_r_objects[in_start + 1];
			}
			else {
				this->_left = in_r_objects[in_start + 1];
				this->_right = in_r_objects[in_start];
			}
		}
		else {
			// まずオブジェクトをソートする
			// 手前の位置にあるオブジェクトがリストの手前に再配置される
			std::sort(in_r_objects.begin() + in_start, in_r_objects.begin() + in_end, comparator);

			// リストの中間インデックスを計算
			auto mid = in_start + object_span / 2;

			// midを基準に左側のリスト、右側のリストに分離して
			// 左右のノードに設定
			this->_left = make_shared<BvhNode>(in_r_objects, in_start, mid, in_time0, in_time1);
			this->_right = make_shared<BvhNode>(in_r_objects, mid, in_end, in_time0, in_time1);
		}

		AABB box_left, box_right;
		// 左右ノードのAABBデータを取得して結合して一つのAABBにしたのを保持する
		bool is_left_boundingbox = this->_left->BoundingBox(in_time0, in_time1, box_left);
		bool is_right_boundingbox = this->_right->BoundingBox(in_time0, in_time1, box_right);
		this->_box = AABB::SurroundingBox(box_left, box_right);
	}

	bool Hit(
		const Ray& in_r_ray, double in_t_min, double in_t_max, hit_record& out_r_rec, int in_skip_object_handle
	) const override
	{
		if (this->_box.hit(in_r_ray, in_t_min, in_t_max) == false)
			return false;

		bool hit_left = this->_left->Hit(in_r_ray, in_t_min, in_t_max, out_r_rec, -1);
		bool hit_right = this->_right->Hit(in_r_ray, in_t_min, in_t_max, out_r_rec, -1);

		return (hit_left || hit_right);
	}

	// AABBによる衝突に必要な情報
	bool BoundingBox(double in_t0, double in_t1, AABB& out_box) const override {
		out_box = this->_box;
		return true;
	}

	inline long Handle() const override {
		return 0;
	}

	inline static bool BoxCompare(
		const shared_ptr<HitTable> a, const shared_ptr<HitTable> b, int axis) {
		AABB box_a;
		AABB box_b;
		if (
			(a->BoundingBox(0, 0, box_a) == false) ||
			(b->BoundingBox(0, 0, box_b) == false)) {
			// コリジョン発生が起きなかった
		}
		// AABBが接触して手前にあるAABBをリストの前にする
		return box_a.Min()._e[axis] < box_b.Min()._e[axis];
	}

	inline static bool BoxXCompare(
		const shared_ptr<HitTable> a, const shared_ptr<HitTable> b) {
		return BoxCompare(a, b, 0);
	}

	inline static bool BoxYCompare(
		const shared_ptr<HitTable> a, const shared_ptr<HitTable> b) {
		return BoxCompare(a, b, 1);
	}

	inline static bool BoxZCompare(
		const shared_ptr<HitTable> a, const shared_ptr<HitTable> b) {
		return BoxCompare(a, b, 2);
	}

public:
	shared_ptr<HitTable> _left;
	shared_ptr<HitTable> _right;
	AABB _box;
};

#endif // __BVH_NODE_H__
