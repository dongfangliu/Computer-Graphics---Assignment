#pragma once

#ifndef _TRIANGLE
#define _TRIANGLE
#endif // !_TRIANGLE

#include "inc\common.h"
#include <vector>
#include<algorithm>
double   const   PI = acos(double(-1));
class edge {
public:
	glm::vec3 p1;
	glm::vec3 p2;

	edge();
	~edge();
	edge(glm::vec3 p1, glm::vec3 p2);
	bool operator==(const edge & edge)const;
	bool has_point(glm::vec3);
};
edge::edge() {
}
edge::~edge() {
}

edge::edge(glm::vec3 p1, glm::vec3 p2) { this->p1 = p1; this->p2 = p2; }
bool edge::operator==(const edge & edge)const {
	if (p1 == edge.p1&& p2 == edge.p2) {
		return true;
	}
	if (p2 == edge.p1&& p1 == edge.p2) {
		return true;
	}
	return false;
}
bool edge::has_point(glm::vec3 p) {
	return (p1 == p) || (p2 == p);
}

class triangle {


public:
	std::vector<edge> edges;
	std::vector<glm::vec3> points;
	triangle();
	~triangle();
	triangle(glm::vec3 p, edge e);
	triangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);
	bool CircumcirleContains(glm::vec3 point);
	bool shareEdge(edge& e);
	bool hasVertex(glm::vec3 & p);
	bool operator==(const triangle & tri)const;

};
triangle::triangle() {}
triangle::~triangle() {}
triangle::triangle(glm::vec3 p, edge e)
{
	points.clear();
	points.push_back(p);
	points.push_back(e.p1);
	points.push_back(e.p2);

	edges.clear();
	edges.push_back(e);
	edges.push_back(edge(p, e.p1));
	edges.push_back(edge(p, e.p2));
}
triangle::triangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
	points.clear(); edges.clear();
	points.push_back(p1); points.push_back(p2); points.push_back(p3);
	edges.push_back(edge(p1, p2));
	edges.push_back(edge(p1, p3));
	edges.push_back(edge(p2, p3));
}

bool triangle::CircumcirleContains(glm::vec3 p)
{
	
	bool same_side = false;
	glm::vec3 p1, p2, p0; p0 = points[0]; p1 = points[1]; p2 = points[2];
	glm::vec3 p1p0 = glm::normalize(p0 - p1);
	glm::vec3 p1p2 = glm::normalize(p2 - p1);

	glm::vec3 normal = glm::abs(glm::normalize(glm::cross(p1p0, p1p2)));
	glm::vec3 pp0 = glm::normalize(p0 - p);
	glm::vec3 pp2 = glm::normalize(p2 - p);

	glm::vec3 normal2 = glm::abs(glm::normalize(glm::cross(pp0, pp2)));
	if (glm::distance(normal, normal2)> 1e-5) {
		return false;
	}

	if (glm::length(p1p0) < 0.000001 || glm::length(p1p2) < 0.000001) {
		return false;
	}
	if (glm::length(pp0) < 0.000001 || glm::length(pp2) < 0.000001) {
		return true;
	}

	float angle_p0p1p2 = acos(glm::dot(p1p0, p1p2));
	float angle_p0pp2 = acos(glm::dot(pp0, pp2));

	if (angle_p0pp2 < 0.000001)
	{
		// 点 P 在线段 P0P1 外，但三点共线
		return false;
	}
	if ((PI - angle_p0pp2) < 0.000001)
	{
		// 点 P 在线段 P0P1 内，三点共线
		return true;
	}
	if (glm::dot(p1p0, p1p2)* glm::dot(pp0, pp2) > 0||(glm::dot(p1p0, p1p2)==0&& glm::dot(pp0, pp2)==0)) {
		same_side = true;
	}
	if (same_side) {
		return angle_p0p1p2 <= angle_p0pp2;
	}
	else {
		return angle_p0p1p2 + angle_p0pp2 >= PI;
	}
}

inline bool triangle::shareEdge(edge& e)
{
	return (edges[0]==e)||(edges[1]==e)||(edges[2]==e);
}
bool triangle::hasVertex(glm::vec3 & p) {
	return (p == points[0]) || (p == points[1]) || (p == points[2]);
}

bool triangle::operator==(const triangle & tri) const
{
	if (points[0] == tri.points[0] || points[0] == tri.points[1] || points[0] == tri.points[2]) {
		if (points[1] == tri.points[0] || points[1] == tri.points[1] || points[1] == tri.points[2]) {
			if (points[2] == tri.points[0] || points[2] == tri.points[1] || points[2] == tri.points[2]) {
				return true;
			}
		}
	}
	return false;
}

std::vector<triangle> DTriangulation(std::vector<glm::vec3> p) {
	int base = sqrt(p.size());
	triangle super_tri[2] = { triangle(p[0], p[base - 1], p[p.size() - base]) ,triangle(p[p.size() - 1], p[base - 1], p[p.size() - base]) };
	std::vector<triangle> tri;
	tri.push_back(super_tri[0]);
	tri.push_back(super_tri[1]);
	p.erase(p.begin() + p.size() - base);
	p.erase(p.begin() + p.size()-1);
	p.erase(p.begin() + base - 1);
	p.erase(p.begin());
	for (int i = 0; i < p.size(); i++) {
		glm::vec3 tmp_point = p[i];
		std::vector<triangle> badTri;
		for (int j = 0; j < tri.size(); j++) {
			if (tri[j].CircumcirleContains(tmp_point)) {
				badTri.push_back(tri[j]);
			}
		}
		std::vector<edge> polygon;
		if (badTri.size() == 1) {
			triangle tri = badTri[0];
			polygon.push_back(tri.edges[0]); polygon.push_back(tri.edges[1]); polygon.push_back(tri.edges[2]);
		}
		else {
			for (int m = 0; m < badTri.size(); m++) {
				triangle tri = badTri[m];
				std::vector<triangle> TRI = badTri;
				TRI.erase(remove(TRI.begin(), TRI.end(), tri), TRI.end());
				for (int k = 0; k < 3; k++) {
					for (int n = 0; n < TRI.size(); n++) {
						if (TRI[n].shareEdge(tri.edges[k])) {
							break;
						}
						if (n == TRI.size() - 1) {
							polygon.push_back(tri.edges[k]);
						}
					}
				}
			}
		}
		for (int j = 0; j < badTri.size(); j++) {
			tri.erase(remove(tri.begin(), tri.end(), badTri[j]), tri.end());
		}

		for (int j = 0; j < polygon.size(); j++) {
			glm::vec3 pp1 = tmp_point - polygon[j].p1;
			glm::vec3 pp2 = tmp_point - polygon[j].p2;
			if(glm::cross(pp1,pp2)!=glm::vec3(0,0,0))
				tri.push_back(triangle(tmp_point, polygon[j]));
		}
		
	}
	//for (std::vector<triangle>::iterator it = tri.begin(); it != tri.end(); it++) {
	//	if (it->hasVertex(super_tri[0].points[0]) || it->hasVertex(super_tri[0].points[1]) || it->hasVertex(super_tri[0].points[2])) {
	//		tri.erase(remove(tri.begin(), tri.end(), *it), tri.end());
	//		continue;
	//	}
	//	if (it->hasVertex(super_tri[1].points[0]) || it->hasVertex(super_tri[1].points[1]) || it->hasVertex(super_tri[1].points[2])) {
	//		tri.erase(remove(tri.begin(), tri.end(), *it), tri.end());
	//		continue;
	//	}
	//}

	for (int i = 0; i < tri.size(); i++) {
		if (tri[i].hasVertex(super_tri[0].points[0]) || tri[i].hasVertex(super_tri[0].points[1]) || tri[i].hasVertex(super_tri[0].points[2])) {
			tri.erase(remove(tri.begin(), tri.end(), tri[i]), tri.end());
			continue;
		}
		if (tri[i].hasVertex(super_tri[1].points[0]) || tri[i].hasVertex(super_tri[1].points[1]) || tri[i].hasVertex(super_tri[1].points[2])) {
			tri.erase(remove(tri.begin(), tri.end(), tri[i]), tri.end());
			continue;
		}

	}

	return tri;
}


int getIndex(glm::vec3 point, std::vector<glm::vec3> P) {
	
	int pos = std::distance(P.begin(), find(P.begin(), P.end(), point));
	return pos;
}