#include "Scene.h"

Collision Scene::RigidCollision(box_data * rb, Vector3d vertex)
{
	Collision coll; coll.axbyczd = -9999;
	for (int i = 0; i < 6; i++) {
		Collision temp = PlaneCollision(rb->plane[i], vertex);
		if (!temp.collide) {
			return Collision();
		}
		else {
			if (temp.axbyczd > coll.axbyczd) {
				coll = temp; 
				coll.rb = vertex - rb->center();
				coll.pb = rb->get_pb(coll.rb);
			}
		}
	}
	return coll;
}

Collision Scene::PlaneCollision(Plane * plane, Vector3d vertex)
{
	double distance = plane->distanceFrom(vertex);
	if (distance < 0.1f) {
		return Collision(distance, plane->normal());
	}
	else {
		return Collision();
	}
}

void Scene::add(Plane *p)
{
	planes.push_back(p);
}

void Scene::add(box_data *rb)
{
	boxes.push_back(rb);
}

Collision Scene::getCollision(box_data * rb, int vertexIndex)
{
	Vector3d vertex = *rb->vertexPosNew[vertexIndex];
	Collision coll;
	for (int i = 0; i < planes.size(); i++) {
		Collision temp = PlaneCollision(planes[i], vertex);
		if (temp.collide&&temp.axbyczd < coll.axbyczd) {
			coll = temp;
			coll.type = hitType::PLANE;
		}
	}
	for (int i = 0; i < boxes.size(); i++) {
		if (boxes[i] == rb) { continue; }
		Collision temp = RigidCollision(boxes[i], vertex);
		if (temp.collide&&temp.axbyczd < coll.axbyczd) {
			coll = temp;
			coll.type = hitType::RIGID;
			
		}
	}
	return coll;
}
