#include "quad.h"
#include <algorithm>
namespace Orchid
{
	Quad::Quad(
		const Vector3d & a,
		const Vector3d & b,
		const Vector3d & c,
		const Vector3d & d,
		const Material & material)
		: _a{ a }
		, _b{ b }
		, _c{ c }
		, _d{ d }
		, _material{ material }

	{
		_normal = Vector3d::cross(a - c, b - d);
		_normal = _normal.normalized();
		double max_x = std::max<double>(std::max<double>(std::max<double>(_a.x(), _b.x()), _c.x()), _d.x());
		double max_y = std::max<double>(std::max<double>(std::max<double>(_a.y(), _b.y()), _c.y()), _d.y());
		double max_z = std::max<double>(std::max<double>(std::max<double>(_a.z(), _b.z()), _c.z()), _d.z());

		double min_x = std::min<double>(std::min<double>(std::min<double>(_a.x(), _b.x()), _c.x()), _d.x());
		double min_y = std::min<double>(std::min<double>(std::min<double>(_a.y(), _b.y()), _c.y()), _d.y());
		double min_z = std::min<double>(std::min<double>(std::min<double>(_a.z(), _b.z()), _c.z()), _d.z());
		this->box.bl = Vector3d(min_x, min_y, min_z);
		this->box.tr = Vector3d(max_x, max_y, max_z);
	}
	ObjectIntersection Quad::getIntersection(const Ray & ray)
	{
		double c = Vector3d::dot(_normal, _a);
		double t = (c - Vector3d::dot(_normal, ray.origin())) / Vector3d::dot(_normal, ray.direction());
		if (t <= 0) {
			return ObjectIntersection(false);
		}
		Vector3d inter_p = ray.origin() + ray.direction()*t;
		double max_x = std::max<double>(std::max<double>(std::max<double>(_a.x(),_b.x()),_c.x()),_d.x());
		double max_y = std::max<double>(std::max<double>(std::max<double>(_a.y(), _b.y()), _c.y()), _d.y());
		double max_z = std::max<double>(std::max<double>(std::max<double>(_a.z(), _b.z()), _c.z()), _d.z());

		double min_x = std::min<double>(std::min<double>(std::min<double>(_a.x(), _b.x()), _c.x()), _d.x());
		double min_y = std::min<double>(std::min<double>(std::min<double>(_a.y(), _b.y()), _c.y()), _d.y());
		double min_z = std::min<double>(std::min<double>(std::min<double>(_a.z(), _b.z()), _c.z()), _d.z());
		if (inter_p.x() >= min_x&&inter_p.x() <= max_x&&inter_p.y() >= min_y&&inter_p.y() <= max_y&&inter_p.z() >= min_z&&inter_p.z() <= max_z) {
			return ObjectIntersection(true, t, _normal, _material);
		}
		else {
			return ObjectIntersection(false);
		}
	}
}
