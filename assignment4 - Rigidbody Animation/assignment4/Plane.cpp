#include "Plane.h"

Plane::Plane()
{
}

Plane::Plane(Vector3d * p1, Vector3d * p2, Vector3d * p3, Vector3d * p4)
{
	points[0] = p1;
	points[1] = p2;
	points[2] = p3;
	points[3] = p4;
}

Vector3d Plane::normal()
{
	Vector3d norm = (*points[2] - *points[0]) % ( *points[3] - *points[1]);
	return norm/sqrt(norm*norm);
}

double Plane::distanceFrom(Vector3d & p)
{
	Vector3d normal = this->normal();
	double d = -(normal*(*points[0]));
	return (p*normal + d) / sqrt(normal*normal);
}
