#include "sphere.h"
namespace  Orchid
{
	Sphere::Sphere(Vector3d position, double radius, Material material)

	{
		_position = position;
		_radius = radius;
		_material = material;
		this->box.tr = position + radius;
		this->box.bl = position - radius;
	}
	ObjectIntersection Sphere::getIntersection(const Ray & ray)
	{
		Vector3d origin = ray.origin() - _position;
		double A = ray.direction().squaredNorm();
		double B = 2 * Vector3d::dot(ray.direction(),origin);
		double C = origin.squaredNorm() - _radius*_radius;
		if (A != 0 && B*B - 4 * A*C >= 0) {
			double t0 = (-B - std::sqrt(B*B - 4 * A*C)) / (2 * A);
			double t1 = (-B + std::sqrt(B*B - 4 * A*C)) / (2 * A);

			if (t0 <= 0 || t1 <= 0) {
				return ObjectIntersection(false);
			}
			else if (std::abs(t0) < std::abs(t1)){
				return ObjectIntersection(true, t0, (ray.origin() + ray.direction()*t0 - _position).normalized(), _material);
			}
			else{
				return ObjectIntersection(true, t1, (ray.origin() + ray.direction()*t1 - _position).normalized(), _material);
			}
		}
		else {
			return ObjectIntersection(false);
		}

	}
}
