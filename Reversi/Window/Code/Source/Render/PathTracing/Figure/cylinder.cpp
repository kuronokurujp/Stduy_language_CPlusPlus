#include "Window/Code/Header/Render/PathTracing/Figure/cylinder.h"

Cylinder::Cylinder(Point3 in_center, const Math::Vec3& in_r_axis, const double in_radius, shared_ptr<Material> in_map_ptr)
{
	this->_Init();

	Math::Vec3 top(in_r_axis * 0.5);
	Math::Vec3 bottom(in_r_axis * -0.5);

	this->_center.Set(in_center);
	this->_pa = this->_center + top;
	this->_pb = this->_center + bottom;
	this->_radius = in_radius;
	this->_map_ptr = in_map_ptr;

	this->_ba = this->_pb - this->_pa;
	this->_baba = Dot(this->_ba, this->_ba);
	this->_inv_baba = 1.0 / this->_baba;
	this->_r2baba = this->_radius * this->_radius * this->_baba;
}

bool Cylinder::Hit(
	const Ray& in_r_ray,
	double in_tmin, double in_tmax,
	hit_record& out_r_rec, int in_skip_object_handle
) const
{
	if (this->_b_hit_enable == false)
		return false;

	Math::Vec3 oc = in_r_ray._orgin - this->_pa;

	double bard = Dot(this->_ba, in_r_ray._unit_dir);
	double baoc = Dot(this->_ba, oc);

	double k2 = this->_baba - bard * bard;
	double k1 = this->_baba * Dot(oc, in_r_ray._unit_dir) - baoc * bard;
	double k0 = this->_baba * Dot(oc, oc) - baoc * baoc - this->_r2baba;

	double h = k1 * k1 - k2 * k0;
	if (h < 0.0)
		return false;

	h = sqrt(h);
	double t = (-k1 - h) / k2;

	// body
	double y = baoc + t * bard;
	if (y > 0.0 && y < this->_baba) {
		if (!IsMinMax(t, in_tmin, in_tmax))
			return false;

		out_r_rec.t = t;
		out_r_rec.p = in_r_ray.At(t);
		Math::Vec3 aa = (oc + t * in_r_ray._unit_dir - this->_ba * y * this->_inv_baba);
		Math::Vec3 outward_normal = aa / this->_radius;

		auto N = UnitVector3(outward_normal);
		out_r_rec.SetFaceNormal(in_r_ray, N);
		out_r_rec.map_ptr = this->_map_ptr;

		out_r_rec.object_handle = this->Handle();

		return true;
	}

	// caps
	t = (((y < 0.0) ? 0.0 : this->_baba) - baoc) / bard;
	if (abs(k1 + k2 * t) < h) {
		if (!IsMinMax(t, in_tmin, in_tmax))
			return false;

		out_r_rec.t = t;
		out_r_rec.p = in_r_ray.At(t);

		double cc = 0.0;
		if (y > 0.0)
			cc = 1.0;
		else if (y < 0.0)
			cc = -1.0;

		Math::Vec3 outward_normal = this->_ba * cc * this->_inv_baba;

		auto N = UnitVector3(outward_normal);
		out_r_rec.SetFaceNormal(in_r_ray, N);
		out_r_rec.map_ptr = this->_map_ptr;

		out_r_rec.object_handle = this->Handle();
		return true;
	}

	return false;
}

// AABBによる衝突に必要な情報
bool Cylinder::BoundingBox(double in_t0, double in_t1, AABB& out_box) const
{
	// 一旦Y軸限定で計算

	// 中心位置から半径への大きさから最大・最小位置を設定
	double min_x = fmin(this->_pb.x() - this->_radius, this->_pa.x() - this->_radius);
	double max_x = fmax(this->_pb.x() + this->_radius, this->_pa.x() + this->_radius);

	double min_y = fmin(this->_pb.y(), this->_pa.y());
	double max_y = fmax(this->_pb.y(), this->_pa.y());

	double min_z = fmin(this->_pb.z() - this->_radius, this->_pa.z() - this->_radius);
	double max_z = fmax(this->_pb.z() + this->_radius, this->_pa.z() + this->_radius);

	// シリンダの上下キャップ位置を設定
	out_box = AABB(
		Math::Vec3(min_x, min_y, min_z),
		Math::Vec3(max_x, max_y, max_z));

	return true;
}