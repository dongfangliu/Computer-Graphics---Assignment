#pragma once
#ifndef _PLANEH_
#define _PLANEH_
#include "3rdparty\Vector.h"
class Plane {
public:
	Plane();
	~Plane() {};
	Plane(Vector3d * p1, Vector3d *p2, Vector3d *p3, Vector3d *p4);
	Vector3d *points[4];
	Vector3d normal();
	double distanceFrom(Vector3d &p);
};
#endif // !_PLANEH_

