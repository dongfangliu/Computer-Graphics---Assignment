#pragma once
#ifndef _BOXDATAH_
#define _BOXDATAH_
#include "../Plane.h"
#include "Rigidstate.h"
#include "stateDot.h"
class box_data {
public:
	Plane * plane[6];
	Vector3d * vertexs[8];
	Vector3d * vertexPosNew[8];
	Rigidstate * rigidState;
	Rigidstate * rigidStateNew;
	StateDot * dot;
	Matrix3x3 Io;
	box_data() {};
	~box_data() {};
	Vector3d center() {
		return (*vertexs[0] + *vertexs[1] + *vertexs[2] + *vertexs[3] + *vertexs[4] + *vertexs[5] + *vertexs[6] + *vertexs[7]) / 8;
	}
	Vector3d get_pb(Vector3d rb) {
		Matrix3x3 R = rigidState->quater.rotation();
		Matrix3x3 I_inverse = R * (Io.inv()) * (R.transpose());
		Vector3d w = I_inverse * rigidState->lamom;
		return dot->velocity + w%rb;
	}
};
#endif // !_BOXDATAH_
