#ifndef _AABBOXH_
#define _AABBOXH_

#include <cfloat>
#include <algorithm>

#include "../math/vector3d.h"
#include "../ray/ray.h"
namespace Orchid
{
	// Axis-aligned bounding box
	class AABBox {
	public:
		Vector3d bl;     // Bottom left (min)
		Vector3d tr;     // Top right   (max)
		AABBox(Vector3d bl_ = Vector3d(), Vector3d tr_ = Vector3d()) {
			bl = bl_, tr = tr_;
		}
		AABBox(Vector3d p0,Vector3d p1,Vector3d p2) {
			float min_x = min3<float>(p0.x(), p1.x(),p2.x());
			float min_y = min3<float>(p0.y(), p1.y(), p2.y());
			float min_z = min3<float>(p0.z(), p1.z(), p2.z());

			float max_x = max3<float>(p0.x(), p1.x(), p2.x());
			float max_y = max3<float>(p0.y(), p1.y(), p2.y());
			float max_z = max3<float>(p0.z(), p1.z(), p2.z());
			this->bl = Vector3d(min_x, min_y, min_z);
			this->tr = Vector3d(max_x, max_y, max_z);
		}
		// Expand to fit box
		void expand(const AABBox &box) {
			if (box.bl.x() < bl.x()) bl[0] = box.bl.x();
			if (box.bl.y() < bl.y()) bl[1] = box.bl.y();
			if (box.bl.z() < bl.z()) bl[2] = box.bl.z();

			if (box.tr.x() > tr.x()) tr[0] = box.tr.x();
			if (box.tr.y() > tr.y()) tr[1] = box.tr.y();
			if (box.tr.z() > tr.z()) tr[2] = box.tr.z();
		}

		// Expand to fit point
		void expand(const Vector3d &vec) {
			if (vec.x() < bl.x()) bl[0] = vec.x();
			if (vec.y() < bl.y()) bl[1] = vec.y();
			if (vec.z() < bl.z()) bl[2] = vec.z();
			if (vec.x() > tr.x()) tr[0] = vec.x();
			if (vec.y() > tr.y()) tr[1] = vec.y();
			if (vec.z() > tr.z()) tr[2] = vec.z();
		}
		// Returns longest axis: 0, 1, 2 for x, y, z respectively
		int get_longest_axis() {
			Vector3d diff = tr - bl;
			if (diff.x() > diff.y() && diff.x() > diff.z()) return 0;
			if (diff.y() > diff.x() && diff.y() > diff.z()) return 1;
			return 2;
		}

		// Check if ray intersects with box. Returns true/false and stores distance in t
		bool intersection(const Ray &r, double &t) {
			double tx1 = (bl.x() - r.origin().x())*r.invdir().x();
			double tx2 = (tr.x() - r.origin().x())*r.invdir().x();

			double tmin = std::min(tx1, tx2);
			double tmax = std::max(tx1, tx2);

			double ty1 = (bl.y() - r.origin().y())*r.invdir().y();
			double ty2 = (tr.y() - r.origin().y())*r.invdir().y();

			tmin = std::max(tmin, std::min(ty1, ty2));
			tmax = std::min(tmax, std::max(ty1, ty2));

			double tz1 = (bl.z() - r.origin().z())*r.invdir().z();
			double tz2 = (tr.z() - r.origin().z())*r.invdir().z();

			tmin = std::max(tmin, std::min(tz1, tz2));
			tmax = std::min(tmax, std::max(tz1, tz2));
			t = tmin;

			return tmax >= tmin;
		}
	/*	bool tri_intersect(const Triangle &tri) {
			Vector3d p0 = tri.get(0); Vector3d p1 = tri.get(1); Vector3d p2 = tri.get(2);

			if (p0.x() >= bl.x()&&p0.x() <= tr.x()&& p0.y() >= bl.y()&&p0.y() <= tr.y() &&p0.z() >= bl.z()&&p0.z() <= tr.z()) {
				return true;
			}
			if (p1.x() >= bl.x()&&p1.x() <= tr.x()&& p1.y() >= bl.y()&&p1.y() <= tr.y() &&p1.z() >= bl.z()&&p1.z() <= tr.z()) {
				return true;
			}
			if (p2.x() >= bl.x()&&p2.x() <= tr.x()&& p2.y() >= bl.y()&&p2.y() <= tr.y() &&p2.z() >= bl.z()&&p2.z() <= tr.z()) {
				return true;
			}
			Vector3d center = (p1 + p2 + p0)/3;
			if (center.x() >= bl.x() && center.x() <= tr.x() && center.y() >= bl.y() && center.y() <= tr.y() && center.z() >= bl.z() && center.z() <= tr.z()) {
				return true;
			}
			if (lineFaceIntersect(p0, p1)) {
				return true;
			}
			if (lineFaceIntersect(p0, p2)) {
				return true;
			}
			if (lineFaceIntersect(p2, p1)) {
				return true;
			}
			return false;
			
		}*/

	private:
		bool lineFaceIntersect(Vector3d & p0, Vector3d & p1) {
			Vector3d cross;
			if (p0.x() != p1.x()) {
				cross = Vector3d(bl.x(), (bl.x() - p0.x()) / (p1.x() - p0.x())*(p1.y() - p0.y()) + p0.y(), (bl.x() - p0.x()) / (p1.x() - p0.x())*(p1.z() - p0.z()) + p0.z());
				if (cross.y() >= bl.y() && cross.y() <= tr.y() && cross.z() >= bl.z() && cross.z() <= tr.z()) {
					return true;
				}
				cross = Vector3d(tr.x(), (tr.x() - p0.x()) / (p1.x() - p0.x())*(p1.y() - p0.y()) + p0.y(), (tr.x() - p0.x()) / (p1.x() - p0.x())*(p1.z() - p0.z()) + p0.z());
				if (cross.y() >= bl.y() && cross.y() <= tr.y() && cross.z() >= bl.z() && cross.z() <= tr.z()) {
					return true;
				}
			}
			if (p0.y() != p1.y()) {
				cross = Vector3d((bl.y() - p0.y()) / (p1.y() - p0.y())*(p1.x() - p0.x()) + p0.x(), bl.y(), (bl.y() - p0.y()) / (p1.y() - p0.y())*(p1.z() - p0.z()) + p0.z());
				if (cross.x() >= bl.x() && cross.x() <= tr.x() && cross.z() >= bl.z() && cross.z() <= tr.z()) {
					return true;
				}
				cross = Vector3d((tr.y() - p0.y()) / (p1.y() - p0.y())*(p1.x() - p0.x()) + p0.x(), tr.y(), (tr.y() - p0.y()) / (p1.y() - p0.y())*(p1.z() - p0.z()) + p0.z());
				if (cross.x() >= bl.x() && cross.x() <= tr.x() && cross.z() >= bl.z() && cross.z() <= tr.z()) {
					return true;
				}
			}
			if (p0.z() != p1.z()) {
				cross = Vector3d((bl.z() - p0.z()) / (p1.z() - p0.z())*(p1.x() - p0.x()) + p0.x(), (bl.z() - p0.z()) / (p1.z() - p0.z())*(p1.y() - p0.y()) + p0.y(), bl.z());
				if (cross.x() >= bl.x() && cross.x() <= tr.x() && cross.y() >= bl.y() && cross.y() <= tr.y()) {
					return true;
				}
				cross = Vector3d((tr.z() - p0.z()) / (p1.z() - p0.z())*(p1.x() - p0.x()) + p0.x(), (tr.z() - p0.z()) / (p1.z() - p0.z())*(p1.y() - p0.y()) + p0.y(), tr.z());
				if (cross.x() >= bl.x() && cross.x() <= tr.x() && cross.y() >= bl.y() && cross.y() <= tr.y()) {
					return true;
				}
			}
			return false;
		}
	};
}

#endif // !_AABBOXH_
