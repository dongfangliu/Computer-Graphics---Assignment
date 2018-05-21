#pragma once
#ifndef _MLBEZIERH_
#define _MLBEZIERH_
#include "../inc/common.h"
#include <vector>
#include <memory>
#include <map>
#include "../triangle.h"

#define MOUSE_SELECT_SENSITIVENESS 5e-2
constexpr  uint32_t controlPointsNums = 16;
class mlBezier
{
public:
	mlBezier();
	~mlBezier();

	glm::vec3 mlEvalBezierCurve(const glm::vec3 *P, const float &t,  int input_length, bool getTagent);
	glm::vec3 mlEvalBezierPatch(const glm::vec3 *controlPoints, const float &u, const float &v);
	glm::vec3 derivBezier(const glm::vec3 *P, const float &t);
	glm::vec3 dUBezier(const glm::vec3 *controlPoints, const float &u, const float &v);
	glm::vec3 dVBezier(const glm::vec3 *controlPoints, const float &u, const float &v);

	void mlCreateBeizermesh();
	void mlTriangularization();

	int getSelectedControlPointIndice(int posX, int posY, glm::mat4 view,glm::mat4 projection, glm::vec4 viewport);
	void getRayScreenToSpace(int posX, int posY, glm::vec3 &outRay_o, glm::vec3 &outRay_d);
	double distancePointToRay(glm::vec3 ray_o, glm::vec3 ray_d, glm::vec3 p);
	void updateControlPointPosition(int selectedInd, glm::vec2 moveDistance, glm::mat4 view, glm::mat4 projection, glm::vec4 viewport);
public:

	uint32_t divs;
	std::vector<glm::vec3> P;
	//std::map<glm::vec3, glm::vec3> N;
	//std::map<glm::vec3, glm::vec2> st;
	std::vector<glm::vec3> N;
	std::vector<glm::vec2> st;

	std::vector<int> indicesofControlpoints;
	std::vector<int> indicesofP;
public:

	glm::vec3 controlPoints[controlPointsNums]
		=
	{
		{ -2.0, -2.0,  1.0 },
		{ -0.5, -2.0,  0.0 },
		{ 0.5, -2.0, -2.0 },
		{ 2.0, -2.0,  2.0 },

		{ -2.0, -0.5,  2.0 },
		{ -0.5, -0.5,  1.5 },
		{ 0.5, -0.5,  0.0 },
		{ 2.0, -0.5, -2.0 },

		{ -2.0,  0.5,  2.0 },
		{ -0.5,  0.5,  1.0 },
		{ 0.5,  0.5, -1.0 },
		{ 2.0,  0.5,  1.0 },

		{ -2.0,  2.0, -1.0 },
		{ -0.5,  2.0, -1.0 },
		{ 0.5,  2.0,  0.0 },
		{ 2.0,  2.0, -0.5 }

	};
	/*	glm::vec3 controlPoints[controlPointsNums]
		=
	{
		{ -2.0, -2.0,  0.0 },
		{ -0.5, -2.0,  0.0 },
		{ 0.5, -2.0, 0.0 },
		{ 2.0, -2.0,  0.0 },

		{ -2.0, -0.5,  0.0 },
		{ -0.5, -0.5,  0 },
		{ 0.5, -0.5,  0.0 },
		{ 2.0, -0.5, 0 },

		{ -2.0,  0.5,  0 },
		{ -0.5,  0.5,  0 },
		{ 0.5,  0.5, 0 },
		{ 2.0,  0.5,  0 },

		{ -2.0,  2.0, 0 },
		{ -0.5,  2.0, 0 },
		{ 0.5,  2.0,  0.0 },
		{ 2.0,  2.0, -0.0 }

	};*/
};

mlBezier::mlBezier()
{
}

mlBezier::~mlBezier()
{
}

glm::vec3 mlBezier::mlEvalBezierCurve(const glm::vec3 * P, const float & t ,int input_length,bool getTagent)
{
	if (input_length == 2) {
		if (getTagent==true) {
			return glm::normalize(P[0] - P[1]);
			
		}else {
			return derivBezier(P,t);
		}
	}else {
		glm::vec3 * out_Points = new glm::vec3[input_length - 1];
		glm::vec3 P1, P2;
		for (int i = 0; i < input_length-1; i++) {
			P1 = P[i]; P2 = P[i + 1];
			out_Points[i] = (1 - t)*P1 + t*P2;
		}
		return mlEvalBezierCurve(out_Points, t,input_length-1,getTagent);
	}
}

glm::vec3 mlBezier::mlEvalBezierPatch(const glm::vec3 * controlPoints, const float & u, const float & v)
{
	glm::vec3 Pu[4];
	for (int i = 0; i < 4;i++) {
		glm::vec3 curveP[4];
		curveP[0] = controlPoints[i * 4];
		curveP[1] = controlPoints[i * 4+1];
		curveP[2] = controlPoints[i * 4+2];
		curveP[3] = controlPoints[i * 4+3];
		Pu[i] = mlEvalBezierCurve(curveP, u,4,false);
	}
	return mlEvalBezierCurve(Pu, v,4,false);
}

inline glm::vec3 mlBezier::derivBezier(const glm::vec3 * P, const float & t)
{
	
	return (1 - t)*P[0] + t*P[1];
}

glm::vec3 mlBezier::dUBezier(const glm::vec3 * controlPoints, const float & u, const float & v)
{
	glm::vec3 Pu[4];
	for (int i = 0; i < 4; i++) {
		glm::vec3 curveP[4];
		curveP[0] = controlPoints[i * 4];
		curveP[1] = controlPoints[i * 4 + 1];
		curveP[2] = controlPoints[i * 4 + 2];
		curveP[3] = controlPoints[i * 4 + 3];
		Pu[i] = mlEvalBezierCurve(curveP, u, 4,true);
	}
	int pre = floor(v * 4);
	float t = v * 4 - pre;
	int next = ceil(v * 4);
	return Pu[pre] * (1 - t) + t*Pu[next];
}

glm::vec3 mlBezier::dVBezier(const glm::vec3 * controlPoints, const float & u, const float & v)
{
	glm::vec3 Pv[4];
	for (int i = 0; i < 4; i++) {
		glm::vec3 curveP[4];
		curveP[0] = controlPoints[i];
		curveP[1] = controlPoints[i + 4];
		curveP[2] = controlPoints[i + 8];
		curveP[3] = controlPoints[i + 12];
		Pv[i] = mlEvalBezierCurve(curveP, v,4, true);
	}
	int pre = floor(u * 4);
	float t = u* 4 - pre;
	int next = ceil(u * 4);
	return Pv[pre] * (1 - t) + t*Pv[next];
}



void mlBezier::mlCreateBeizermesh()
{
	P.clear(); N.clear(); st.clear();
	float step = 1/sqrt(divs);
	for (float v = 0; v <= 1; v += step) {
		for (float u = 0; u <= 1; u += step) {
			glm::vec3 p = mlEvalBezierPatch(controlPoints, u, v);
			P.push_back(p);
			N.push_back(
				glm::normalize(
					glm::cross(dUBezier(controlPoints, u, v), dVBezier(controlPoints, u, v))
				)
			);
			st.push_back(glm::vec2(u, v));

		}
	}

}

void mlBezier::mlTriangularization()
{
	indicesofControlpoints.clear();
	for (int j = 0; j < 3; j++)
	{
		for (int i = 0; i < 3; i++)
		{
			int ind = j * 4 + i;
			indicesofControlpoints.push_back(ind);
			indicesofControlpoints.push_back(ind + 1);
		
			

			indicesofControlpoints.push_back(ind + 5);
			indicesofControlpoints.push_back(ind + 4);
		}
	}
	indicesofP.clear();
	int div = sqrt(divs);
	indicesofP.clear();
	for (int j = 0; j < div; j++)
	{
		for (int i = 0; i < div; i++)
		{
			int ind = j * (div+1) + i;
			indicesofP.push_back(ind);
			indicesofP.push_back(ind + 1);
			indicesofP.push_back(ind + (div + 1));

			indicesofP.push_back(ind + 1);
			indicesofP.push_back(ind + (div + 2));
			indicesofP.push_back(ind + (div + 1));
		}
	}
}




// you will fill the four function to edit control points
int mlBezier::getSelectedControlPointIndice(int posX, int posY,glm::mat4 view,glm::mat4 projection,glm::vec4 viewport)
{
	float minDistance = 65536;
	int ind = -1;
	float thres = 20;
	for (int i = 0; i < controlPointsNums; i++) {
		glm::vec3 pos = glm::project(controlPoints[i], view, projection, viewport);
		float distance =glm::distance(glm::vec2(posX, viewport[3] - posY), glm::vec2(pos.x,pos.y));
		if (distance < minDistance && distance<thres) {
			minDistance = distance;
			ind = i;
		}
	}
	return ind;
}

void mlBezier::getRayScreenToSpace(int posX, int posY, glm::vec3 &outRay_o, glm::vec3 &outRay_d)
{
	
}

double mlBezier::distancePointToRay(glm::vec3 ray_o, glm::vec3 ray_d, glm::vec3 p)
{
	return 0;
}

void mlBezier::updateControlPointPosition(int selectedInd,glm::vec2 moveDistance,glm::mat4 view, glm::mat4 projection, glm::vec4 viewport)
{
	glm::vec3 pos = glm::project(controlPoints[selectedInd], view, projection, viewport);
	glm::vec3 newprojected = glm::vec3(pos.x + moveDistance.x, pos.y + moveDistance.y, pos.z);
	this->controlPoints[selectedInd] = glm::unProject(newprojected,view, projection, viewport);
	
}

#endif // !_MLBEZIERH_
