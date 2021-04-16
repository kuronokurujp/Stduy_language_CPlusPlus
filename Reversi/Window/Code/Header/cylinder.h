#ifndef __CYLINDER_H__
#define __CYLINDER_H__

#include "hit_table.h"
#include "vec3.h"

// shader_ptrなどで使う
#include <memory>

#include "combaseapi.h"

class Material;

class Cylinder : public HitTable
{
public:
	Cylinder() { this->_Init(); }
	Cylinder(Point3 in_center, const Math::Vec3& in_r_axis, const double in_radius, shared_ptr<Material> in_map_ptr) {
		this->_Init();

		Math::Vec3 top(in_r_axis * 0.5);
		Math::Vec3 bottom(in_r_axis * -0.5);

		this->_center.Set(in_center);
		this->_pa = this->_center + top;
		this->_pb = this->_center + bottom;
		this->_radius = in_radius;
		this->_map_ptr = in_map_ptr;

		this->_ba = this->_pb - this->_pa;
	}

	bool Hit(
		const Ray& in_r_ray, double in_tmin, double in_tmax, hit_record& out_r_rec, int in_skip_object_handle
	) const override {
		Math::Vec3 rd(UnitVector3(in_r_ray._dir));
		Math::Vec3 oc = in_r_ray._orgin - this->_pa;

		//Math::Vec3 ba = this->_pb - this->_pa;

		double baba = Dot(this->_ba, this->_ba);
		double bard = Dot(this->_ba, rd);
		double baoc = Dot(this->_ba, oc);

		double k2 = baba - bard * bard;
		double k1 = baba * Dot(oc, rd) - baoc * bard;
		double k0 = baba * Dot(oc, oc) - baoc * baoc - this->_radius * this->_radius * baba;

		double h = k1 * k1 - k2 * k0;
		if (h < 0.0)
			return false;

		h = sqrt(h);
		double t = (-k1 - h) / k2;

		// body
		double y = baoc + t * bard;
		if (y > 0.0 && y < baba) {
			if (!is_0X(t, in_tmin, in_tmax))
				return false;

			out_r_rec.t = t;
			out_r_rec.p = in_r_ray.At(t);
			Math::Vec3 aa = (oc + t * rd - this->_ba * y / baba);
			Math::Vec3 outward_normal = aa / this->_radius;

			auto N = UnitVector3(outward_normal);
			out_r_rec.SetFaceNormal(in_r_ray, N);
			out_r_rec.map_ptr = this->_map_ptr;

			out_r_rec.object_handle = this->Handle();

			return true;
		}

		// caps
		t = (((y < 0.0) ? 0.0 : baba) - baoc) / bard;
		if (abs(k1 + k2 * t) < h) {
			if (!is_0X(t, in_tmin, in_tmax))
				return false;

			out_r_rec.t = t;
			out_r_rec.p = in_r_ray.At(t);

			double cc = 0.0;
			if (y > 0.0)
				cc = 1.0;
			else if (y < 0.0)
				cc = -1.0;

			Math::Vec3 outward_normal = this->_ba * cc / baba;

			auto N = UnitVector3(outward_normal);
			out_r_rec.SetFaceNormal(in_r_ray, N);
			out_r_rec.map_ptr = this->_map_ptr;

			out_r_rec.object_handle = this->Handle();
			return true;
		}

		return false;
	}

	//0からmaxまでにxが入ってるかどうか
	static inline bool is_0X(double x, double min, double max) {
		return (min < x&&x < max);
	}

	long Handle() const override {
		return this->_handle;
	}

private:
	Point3 _center;
	Point3 _pa;
	Point3 _pb;
	double _radius;

	long _handle;

	shared_ptr<Material> _map_ptr;

private:
	Math::Vec3 _ba;

	void _Init() {
		// オブジェクトのユニークなIDを生成する
		// オブジェクトの識別に利用
		GUID guid;
		CoCreateGuid(&guid);
		this->_handle = guid.Data1;
	}
};

#endif // __CYLINDER_H__
