
#ifndef __RIGIDSTATE_H__
#define __RIGIDSTATE_H__

#include "../3rdparty/quaternion.h"
#include "../3rdparty/Vector.h"
#include "stateDot.h"
#include "Rigidstate.h"
#include "../Plane.h"
#include "Collision.h"
#include "Scene.h"
#include <vector>
const float thr = 0.25;
const int number = 8;
const float mass = 1.0;
const float size_ = 5.0;
const float halfSize = 2.5;
const float wallY = 0;
const double Cr = 0.5;
const double Ct = 0.9;
//float const DepthEpsilon = 0.001f;
float const DepthEpsilon = 0.1f;
double const updateTime = 0.005f;

class RigidBody
{
public:
	Scene * scene;
	Plane plane[6];
	Vector3d vertexPos[number];
	Vector3d vertexPosNew[number];
	box_data data;

	RigidBody(Vector3d,Scene *scene);
	~RigidBody();
	RigidBody() {};
	void updateFall();
	void startFall();
	Matrix3x3 getStateRotation();
	Vector3d getStatePosition();
private:
	bool  resetSign = false;
	Matrix3x3 Io = Matrix3x3(mass*1.0 / 12 * (size_*size_ * 2), 0, 0, 0, mass*1.0 / 12 * (size_*size_ * 2), 0, 0, 0, mass*1.0 / 12 * (size_*size_ * 2));

	Rigidstate  rigidStateNew;
	Rigidstate  rigidState;
	StateDot    rigidStateDot;

	Vector3d center = Vector3d(0, 0, 0);
	Vector3d bodyForce = Vector3d(0, 0, 0);
	Vector3d vertexForce[8] = { Vector3d(0,0,0),Vector3d(0,0,0),Vector3d(0,0,0),Vector3d(0,0,0),
		Vector3d(0,0,0),Vector3d(0,0,0),Vector3d(0,0,0),Vector3d(0,0,0) };
	Vector3d Vpn = Vector3d(0, 1, 0);

	StateDot F(Rigidstate& rigidState);
	void statesNumInt(Rigidstate& rigidState, StateDot& rigidStateDot, Rigidstate& rigidStateNew, double h);
	void collisionDetect(Rigidstate& rigidState, StateDot& rigidStateDot, Rigidstate& rigidStateNew, double h);
	void updatePos(double h, bool straightUpdate);
	void ResolveCollisions(int cornerIndex);
	Vector3d get_w(Rigidstate & rigidState);
	bool checkForCollision();
	void comfirmChange();
	bool hasPlane(Plane * p);
};


#endif
