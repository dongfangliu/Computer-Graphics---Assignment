

#include "../sample/sampler.h"

namespace Orchid
{

	namespace helper
	{
		void calcLocalCoords(const Vector3d& w, Vector3d* u, Vector3d* v) {

		}
	}
	namespace sampler
	{
		void onHemisphere(const Vector3d& normal, Vector3d* direction, double &r1, double &r2, float * attentu)
		{
			Vector3d up_vec = Vector3d(0, 1, 0);
			Vector3d x_hat, y_hat, z_hat;
			if (normal.normalized() == up_vec) {
				x_hat = Vector3d(1, 0, 0);
				y_hat = Vector3d(0, 0, -1);
				z_hat = Vector3d(0, 1, 0);
			}
			else if (normal.normalized() == -up_vec) {
				x_hat = Vector3d(1, 0, 0);
				y_hat = Vector3d(0, 0, 1);
				z_hat = Vector3d(0, -1, 0);
			}
			else {
				x_hat = Vector3d::cross(normal, up_vec);
				y_hat = Vector3d::cross(x_hat, normal);
				z_hat = normal;
			}
			x_hat = x_hat.normalized(); y_hat = y_hat.normalized(); z_hat = z_hat.normalized();

			double theta = 0.5*acos(1 - 2 * r1);
			/*double theta = acos(r1);*/
			double phi = 2 * PI*r2;
			Vector3d sampled_point = Vector3d(sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta));
			*attentu = cos(theta);

			Vector3d line1 = Vector3d(x_hat.x(), y_hat.x(), z_hat.x());
			Vector3d line2 = Vector3d(x_hat.y(), y_hat.y(), z_hat.y());
			Vector3d line3 = Vector3d(x_hat.z(), y_hat.z(), z_hat.z());

			double x = line1.dot(sampled_point); double y = line2.dot(sampled_point); double z = line3.dot(sampled_point);

			*direction = Vector3d(x, y, z).normalized();
		}

		void onTriangle(const Triangle& tri, Vector3d* position)
		{
			unsigned short a[3];
			double r1 = erand48(a); double r2 = erand48(a);
			while (r1 + r2 >= 1) {
				r1 = erand48(a);  r2 = erand48(a);
			}
			*position = r1*tri.get(0) + r2*tri.get(1) + (1 - r1 - r2)*tri.get(2);
		}

	}
}
