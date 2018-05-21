#ifdef _MSC_VER
#pragma once
#endif // _MSC_VER

#include "../core/common.h"
#include "../math/vector3d.h"
#include "../objects/shape.h"
#include "../erand48.h"
namespace Orchid
{
	
	namespace helper
	{
		void calcLocalCoords(const Vector3d& w, Vector3d* u, Vector3d* v);
	}
	namespace sampler
	{
		void onHemisphere(const Vector3d& normal, Vector3d* direction, double &r1, double &r2, float * attentu);

		void onTriangle(const Triangle& tri, Vector3d* position);

	}
}
