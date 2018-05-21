#include "triangle.h"

namespace Orchid
{
	Triangle::Triangle()
	{

	}
	Triangle::Triangle(const Vector3d & p0, const Vector3d & p1, const Vector3d & p2, Material material)
	{
		_p0 = p0;
		_p1 = p1;
		_p2 = p2;
		_material = material;
		this->box = AABBox(p0,p1,p2);
	}
	Triangle::Triangle(const Triangle & tri)
	{
		this->operator=(tri);
	}
	Triangle::~Triangle()
	{
	}
	Triangle & Triangle::operator=(const Triangle & tri)
	{
		// TODO: insert return statement here
		this->_p0 = tri._p0;
		this->_p1 = tri._p1;
		this->_p2 = tri._p2;
		this->_material = tri._material;
		return *this;
	}
	Vector3d Triangle::gravity() const
	{
		return Vector3d();
	}
	AABBox Triangle::get_bounding_box()
	{
		
		return this->box;
	}
	bool Triangle::triangleInBbox()
	{
		return false;
	}
	Vector3d Triangle::get(int id) const
	{
		Assertion(0 <= id && id <= 2, "ID must be between 0 and 2");
		if (0 == id)
			return _p0;
		if (1 == id)
			return _p1;
		if (2 == id)
			return _p2;
	}
	Vector3d Triangle::normal() const
	{
		return Vector3d::cross(get(1) - get(0), get(2) - get(0)).normalized();
	}
	ObjectIntersection Triangle::getIntersection(const Ray & ray)
	{
		Vector3d e1 = get(1) - get(0);
		Vector3d e2 = get(2) - get(0);
		Vector3d s = ray.origin() - get(0);
		Vector3d s2 = Vector3d::cross(s, e1);
		Vector3d s1 = Vector3d::cross(ray.direction(), e2);
		double t = 1 / Vector3d::dot(s1, e1) *Vector3d::dot(s2, e2);
		double b1 = 1 / Vector3d::dot(s1, e1)*Vector3d::dot(s1, s);
		double b2 = 1 / Vector3d::dot(s1, e1)*Vector3d::dot(s2, ray.direction());
		if (b1>0 &&b2>0&&b1 + b2 < 1&&t>0) {
			return ObjectIntersection(true, t, this->normal(), this->_material);
		}
		else {
			return ObjectIntersection(false);
		}
	}
	double Triangle::area() const
	{
		const Vector3d e1 = _p1 - _p0;
		const Vector3d e2 = _p2 - _p0;
		return 0.5 * Vector3d::cross(e1, e2).norm();
	}
	void Triangle::normalize(double maxLength)
	{
		_p0 /= maxLength;
		_p1 /= maxLength;
		_p2 /= maxLength;
		this->box = AABBox(_p0, _p1, _p2);
	}

	void Triangle::translate(Vector3d t)
	{
		_p0 += t;
		_p1 += t;
		_p2 += t;
		this->box = AABBox(_p0, _p1, _p2);
	}

	void Triangle::scale(double s)
	{
		_p0 *= s;
		_p1 *= s;
		_p2 *= s;
	}
	
}
