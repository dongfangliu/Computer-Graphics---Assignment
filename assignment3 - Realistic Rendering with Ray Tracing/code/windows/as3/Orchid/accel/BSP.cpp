#include "../BSP.h"
#include<stack>
using namespace std;
namespace Orchid {
	unsigned int expandBits(unsigned int v)
	{
		v = (v * 0x00010001u) & 0xFF0000FFu;
		v = (v * 0x00000101u) & 0x0F00F00Fu;
		v = (v * 0x00000011u) & 0xC30C30C3u;
		v = (v * 0x00000005u) & 0x49249249u;
		return v;
	}
	// given 3D point located within the unit cube [0,1].
	unsigned int bsp_node::morton3D(float x, float y, float z)
	{
		x = min(max(x * 1024.0, 0.0), 1023.0);
		y = min(max(y * 1024.0, 0.0), 1023.0);
		z = min(max(z * 1024.0, 0.0), 1023.0);
		unsigned int xx = expandBits((unsigned int)x);
		unsigned int yy = expandBits((unsigned int)y);
		unsigned int zz = expandBits((unsigned int)z);
		return xx * 4 + yy * 2 + zz;

	}
	int _clz(unsigned int x) {
		int i = 0;
		while (i < 30) {
			if (x >> i == 0) {
				return 30 - i;
			}
			i++;
		}
		return 0;
	}
	int findSplit(unsigned int* sortedMortonCodes, int first, int  last)
	{
		// Identical Morton codes => split the range in the middle.

		unsigned int firstCode = sortedMortonCodes[first];
		unsigned int lastCode = sortedMortonCodes[last];

		if (firstCode == lastCode)
			return (first + last) >> 1;

		// Calculate the number of highest bits that are the same
		// for all objects, using the count-leading-zeros intrinsic.

		int commonPrefix = _clz(firstCode ^ lastCode);

		// Use binary search to find where the next bit differs.
		// Specifically, we are looking for the highest object that
		// shares more than commonPrefix bits with the first one.

		int split = first; // initial guess
		int step = last - first;

		do
		{
			step = (step + 1) >> 1; // exponential decrease
			int newSplit = split + step; // proposed new position

			if (newSplit < last)
			{
				unsigned int splitCode = sortedMortonCodes[newSplit];
				int splitPrefix = _clz(firstCode ^ splitCode);
				if (splitPrefix > commonPrefix)
					split = newSplit; // accept proposal
			}
		} while (step > 1);

		return split;
	}

	bsp_node * generateHierarchy(unsigned int* sortedMortonCodes, const vector<Triangle*> & tris, int first, int last)
	{
		// Single object => create a leaf node.

		if (first == last)
			return new bsp_node(tris[first]);

		// Determine where to split the range.

		int split = findSplit(sortedMortonCodes, first, last);

		// Process the resulting sub-ranges recursively.

		bsp_node* childA = generateHierarchy(sortedMortonCodes, tris,
			first, split);
		bsp_node* childB = generateHierarchy(sortedMortonCodes, tris,
			split + 1, last);
		return new bsp_node(childA, childB);
	}
	inline bsp_node::bsp_node(Triangle * tri)
	{
		this->tri = tri;
		this->box = tri->get_bounding_box();
	}
	bsp_node::bsp_node(bsp_node * left, bsp_node * right) {
		this->left = left;
		this->right = right;
		this->box = left->box;
		this->box.expand(right->box);
		this->tri = NULL;
	}

	inline bool comp_tri(Triangle* t1, Triangle * t2) {
		return t1->morton_code <= t2->morton_code;
	}
	bsp_node * bsp_node::getBox(vector<Triangle*> & tris) {
		unsigned int * MortonCodes = new unsigned int[tris.size()];
		for (int i = 0; i < tris.size(); i++) {
			MortonCodes[i]= tris[i]->morton_code;
		}
		sort(MortonCodes, MortonCodes + tris.size());
		sort(tris.begin(), tris.end(), comp_tri);
		bsp_node * box = generateHierarchy(MortonCodes, tris, 0, tris.size() - 1);
		free(MortonCodes);
		return box;
	}

	ObjectIntersection bsp_node::traverse(const Ray & ray)
	{
		vector<Triangle *> hitList;
		stack<bsp_node *> stack;
		bsp_node * node = this;
		do {
			double t1, t2;
			bsp_node * childL = node->left;
			bsp_node * childR = node->right;
			bool hitLeft = childL->box.intersection(ray, t1);
			bool hitRight = childR->box.intersection(ray, t2);
			if (isLeaf(childL) && hitLeft) {
				if (childL->tri->getIntersection(ray)._hit) {
					hitList.push_back(childL->tri);
				}
			}
			if (isLeaf(childR) && hitRight) {
				if (childR->tri->getIntersection(ray)._hit) {
					hitList.push_back(childR->tri);
				}
			}
			bool traverseL = (hitLeft && !isLeaf(childL));
			bool traverseR = (hitRight && !isLeaf(childR));
			if (!traverseL && !traverseR){
				if (!stack.empty()) {
					node = stack.top();
					stack.pop();
				}
				else {
					node = NULL; 
				}
			}else
			{
				node = (traverseL) ? childL : childR;
				if (traverseL && traverseR)
					stack.push(childR); // push
			}

		} while (node != NULL);
		ObjectIntersection intersect(false); intersect._u = 99999999999999999;
		if (hitList.size() < 1) {
			return ObjectIntersection(false);
		}
		for (size_t i = 0; i < hitList.size(); i++) {
			ObjectIntersection temp = hitList.at(i)->getIntersection(ray);
			if (temp._hit && temp._u < intersect._u) {
				intersect = temp;
			}
		}
		return intersect;
	}

	bool bsp_node::isLeaf(bsp_node * n)
	{
		return n->left == NULL && n->right == NULL;
	}


}