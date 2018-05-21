#include "rigidBody.h"

RigidBody::RigidBody(Vector3d center,Scene *  scene)
{
	this->scene = scene;

	vertexPos[0] = Vector3d(-halfSize, -halfSize, halfSize) + center;
	vertexPos[1] = Vector3d(halfSize, -halfSize, halfSize) + center;
	vertexPos[2] = Vector3d(halfSize, -halfSize, -halfSize) + center;
	vertexPos[3] = Vector3d(-halfSize, -halfSize, -halfSize) + center;
	vertexPos[4] = Vector3d(-halfSize, halfSize, halfSize) + center;
	vertexPos[5] = Vector3d(halfSize, halfSize, halfSize) + center;
	vertexPos[6] = Vector3d(halfSize, halfSize, -halfSize) + center;
	vertexPos[7] = Vector3d(-halfSize, halfSize, -halfSize) + center;

	plane[0] = Plane(vertexPos, vertexPos + 1, vertexPos + 5, vertexPos + 4);
	plane[1] = Plane(vertexPos+3, vertexPos + 7, vertexPos + 6, vertexPos + 2);
	plane[2] = Plane(vertexPos+7, vertexPos + 4, vertexPos + 5, vertexPos + 6);
	plane[3] = Plane(vertexPos + 3, vertexPos + 2, vertexPos + 1, vertexPos + 0);
	plane[4] = Plane(vertexPos + 2, vertexPos + 6, vertexPos + 5, vertexPos + 1);
	plane[5] = Plane(vertexPos + 3, vertexPos + 0, vertexPos + 4, vertexPos + 7);

	for (int i = 0; i < number; i++) {
		vertexPosNew[i] = vertexPos[i];
	}
	for (int i = 0; i < 6; i++) {
		data.plane[i] = plane + i;
	}
	for (int i = 0; i < 8; i++) {
		data.vertexs[i] = vertexPos + i;
		data.vertexPosNew[i] = vertexPosNew + i;
	}

	//Matrix3x3 m(0, 0, 0, 0, 0, 0, 0, 0, 0);
	Matrix3x3 m(1, 0, 0, 0, 1, 0, 0, 0, 1);
	rigidState.xposition = center;
	rigidState.quater = Quaternion(m);
	rigidState.pfmom = Vector3d(0, 0, 0); 
	rigidState.lamom = Vector3d(0, 0, 0);

	Vector3d w(0, 0, 0);
	rigidStateDot.velocity = Vector3d(0, 0, 0);
	rigidStateDot.quaterA = 0.5*w*(rigidState.quater);
	rigidStateDot.force = Vector3d(0, 0, 0);
	rigidStateDot.torque = Vector3d(0, 0, 0);

	data.rigidState = &rigidState; data.rigidStateNew = &rigidStateNew; data.dot = &rigidStateDot; data.Io = Io;
}

RigidBody::~RigidBody()
{
}

void RigidBody::startFall()
{
	resetSign = false;

	bodyForce = Vector3d(0, -10, 0);
}

Matrix3x3 RigidBody::getStateRotation()
{
	Matrix3x3 m = rigidState.quater.rotation();
	return m;
}

Vector3d RigidBody::getStatePosition()
{
	return rigidState.xposition;
}



void RigidBody::updateFall()
{
	rigidStateDot = F(rigidState);
	statesNumInt(rigidState, rigidStateDot, rigidStateNew,updateTime);
	collisionDetect(rigidState, rigidStateDot, rigidStateNew,updateTime);
}

StateDot RigidBody::F(Rigidstate& rigidState)// compute force
{
	StateDot temporal_state;

	temporal_state.velocity = rigidState.pfmom / mass;

	Vector3d force = bodyForce + vertexForce[0] + vertexForce[1] + vertexForce[2] + vertexForce[3] + vertexForce[4] + vertexForce[5] + vertexForce[6] + vertexForce[7];
	temporal_state.force = force;

	Matrix3x3 R = rigidState.quater.rotation();
	Matrix3x3 I_inverse = R * (Io.inv()) * (R.transpose());
	Vector3d w = I_inverse * rigidState.lamom;
	temporal_state.quaterA = 0.5*w*rigidState.quater;

	Vector3d torque;
	for (int i = 0; i < 8; i++) {
		torque = torque + (vertexPos[i] - rigidState.xposition) % vertexForce[i];
	}
	temporal_state.torque = torque;
	return  temporal_state;
}
Vector3d RigidBody::get_w(Rigidstate & rigidState) {
	Matrix3x3 R = rigidState.quater.rotation();
	Matrix3x3 I_inverse = R * (Io.inv()) * (R.transpose());
	Vector3d w = I_inverse * rigidState.lamom;
	return w;
}

void RigidBody::statesNumInt(Rigidstate& rigidState, StateDot& rigidStateDot, Rigidstate& rigidStateNew, double h)
{
	rigidStateNew.xposition = rigidState.xposition + h*rigidStateDot.velocity;
	rigidStateNew.pfmom = rigidState.pfmom + h*rigidStateDot.force;
	rigidStateNew.lamom = rigidState.lamom + h*rigidStateDot.torque;
	rigidStateNew.quater = rigidState.quater + h*rigidStateDot.quaterA;
}

void RigidBody::collisionDetect(Rigidstate& rigidState, StateDot& rigidStateDot, Rigidstate& rigidStateNew, double h)
{
	//should already update rigidStateNew and vertexPosNew
	double t = h;
	float axbyczd = 99999;
	//first get t suitable for all vertexs.
	for (int i = 0; i < 8; i++) {
		if (resetSign) { i = 0;
		resetSign = false;
		}
		updatePos(t, false);
		Collision collision = scene->getCollision(&this->data, i);
		while (collision.axbyczd < -DepthEpsilon)
		{
			//decrease the current delta t and update vertex position and velocity
			t = t / 2;
			updatePos(t, false);collision = scene->getCollision(&this->data, i);
			resetSign = true;
		}	
	}
	//Then consider collision on each vertex.

	for (int i = 0; i < 8; i++) {
		updatePos(t, true);
		Collision collision = scene->getCollision(&this->data, i);
		if (collision.axbyczd > DepthEpsilon) {
			continue;
		}
		StateDot dot = F(rigidStateNew);

		Vector3d ra = (vertexPosNew[i] - rigidStateNew.xposition);
		Vector3d pa = dot.velocity + get_w(rigidStateNew) % ra;
		
		Vector3d pb(0, 0, 0);
		Vector3d rb(0, 0, 0);
		if (collision.type == RIGID) {
			pb = collision.pb; rb = collision.rb;
		}

		double reve = collision.normal*(pa - pb);
		if (reve < 0) {
			double epsilon = 0.6;
			double j;
			if (collision.type == RIGID) {
				j = -(1 + epsilon)*reve*1.5 /
					(1 / mass + 1/mass + collision.normal*((Io.inv()*(ra%collision.normal)) % ra) + collision.normal*((Io.inv()*(collision.rb%collision.normal)) % collision.rb));
			}
			else {
				j = -(1 + epsilon)*reve /
					(1 / mass + 0 + collision.normal*((Io.inv()*(ra%collision.normal)) % ra) + 0);
			}
			vertexForce[i] = j / h*collision.normal;
		}
	}
	//Now update status 
	updatePos(t, false); comfirmChange();
	for (int i = 0; i < 8; i++) {
		vertexForce[i] = 0;
	}


}

bool RigidBody::checkForCollision()
{
	return false;
}
void RigidBody::updatePos(double h, bool straightUpdate)
{
	if (!straightUpdate) {
		rigidStateDot = F(rigidState);
	}

	statesNumInt(rigidState, rigidStateDot, rigidStateNew, h);
	Vector3d centerNew = rigidStateNew.xposition;
	for (int i = 0; i < 8; i++) {
		Matrix3x3 R = rigidStateNew.quater.rotation();
		vertexPosNew[i] = R * (vertexPos[i] - rigidState.xposition) + centerNew;
	}
}

void RigidBody::ResolveCollisions(int cornerIndex)
{

}
void RigidBody::comfirmChange() {
	rigidState = rigidStateNew;
	rigidStateDot = F(rigidState);
	for (int i = 0; i < 8; i++) {
		vertexPos[i] = vertexPosNew[i];
	}
}

bool RigidBody::hasPlane(Plane * p)
{
	for (int i = 0; i < 6; i++) {
		if (p == plane + i) {
			return true;
		}
	}
	return false;
}
