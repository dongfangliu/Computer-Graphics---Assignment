#ifndef  _COLLISIONH_
#define _COLLISIONH_
#include "../3rdparty/Vector.h"
enum hitType{PLANE,RIGID};
class Collision {
public:
	Collision() {};
	Collision(double x, Vector3d y) :axbyczd(x), normal(y) { collide = true; }
	double axbyczd = 9999;
	Vector3d normal;
	bool collide = false;
	hitType type = PLANE;
	Vector3d pb;
	Vector3d rb;

};
#endif // ! _COLLISIONH_

