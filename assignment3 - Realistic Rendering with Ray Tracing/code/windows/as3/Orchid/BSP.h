#pragma once
#include "accel\aabbox.h"
#include <vector>
#include "objects\triangle.h"
#include <numeric>
using namespace std;
namespace Orchid {
	class bsp_node {
	public:
		bsp_node * left = NULL;
		bsp_node * right = NULL;
		Triangle* tri=NULL;
		AABBox box;
		bsp_node(Triangle * tri);
		bsp_node(bsp_node * left, bsp_node * right);
		static bsp_node * getBox(vector<Triangle*> & tris);
		ObjectIntersection traverse(const Ray & ray);
		bool isLeaf(bsp_node *);
		static unsigned int morton3D(float x, float y, float z);
	};
	
}