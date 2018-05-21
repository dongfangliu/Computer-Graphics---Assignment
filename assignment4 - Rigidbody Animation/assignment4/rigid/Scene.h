#pragma once
#ifndef _SCENEH_
#define _SCENEH_
#include "Collision.h"
#include "../Plane.h"
#include "box_data.h"
#include <vector>
class Scene {
private :
	vector<Plane *> planes;
	vector<box_data *> boxes;
	Collision RigidCollision(box_data * rb, Vector3d vertex);
	Collision PlaneCollision(Plane * plane, Vector3d vertex);
public :
	Scene() {};
	void add(Plane *);
	void add(box_data *);
	Collision getCollision(box_data * rb, int vertexIndex);

};
#endif // !_SCENEH_
