#include "scene.h"
#include "../sample/sampler.h"
namespace Orchid
{
	const int max_depth = 4;
	
	void Scene::add(Object * object)
	{
		_objects.push_back(object);
	}

	void Scene::gen_lightSamples(int sampleNum)
	{
		Quad * quad = (Quad *)_objects[0];
		Triangle t1 = Triangle(quad->_a, quad->_b, quad->_c, quad->_material);
		Triangle t2 = Triangle(quad->_a, quad->_d, quad->_c, quad->_material);
		for (int i = 0; i < sampleNum / 2; i++) {
			Vector3d p;
			sampler::onTriangle(t1, &p);
			lightSamples.push_back(p);
			sampler::onTriangle(t2, &p);
			lightSamples.push_back(p);
		}
	}

	ObjectIntersection Scene::intersect(const Ray & ray, int * hit_id)
	{
		ObjectIntersection isct = ObjectIntersection();
		ObjectIntersection temp;
		long size = _objects.size();
		for (int i = 0; i < size; i++)
		{
			temp = _objects.at((unsigned)i)->getIntersection(ray);

			if (temp._hit) {
				if (isct._u == 0 || temp._u < isct._u) { 
					isct = temp; 
					*hit_id = i;
				}
			}
		}
		return isct;
	}

	Vector3d Scene::trace_ray(const Ray & ray, int depth, unsigned short * Xi)
	{
		
		bool inShadow = false; int hit_id = 0;
		ObjectIntersection intersect = this->intersect(ray, &hit_id);
		if (depth > max_depth) {
			return Vector3d(0, 0, 0);
		}
		
		if (intersect._hit) {
			if (ray.attenuation <= 0.001) {
				return Vector3d(0, 0, 0);
			}
			if (intersect._material.getType() == EMIT) {
				return intersect._material.get_emission();
			}
			Vector3d direct_color = shadowRay(ray, intersect, hit_id); // if shadow count 0 ,else,count light energy.
			Vector3d diffuse_color = intersect._material.get_colour();

			Ray reflect_ray = intersect._material.get_reflected_ray(ray, ray.origin() + intersect._u*ray.direction(), intersect._normal, Xi);
			Vector3d indirect_color = trace_ray(reflect_ray, depth + 1, Xi);
			Vector3d reflect_color = direct_color*0.1 + diffuse_color*indirect_color*ray.attenuation;
			if (intersect._material.getType() == REFRC) {
				Vector3d refract_color(0, 0, 0);
				float kr=0;
				intersect._material.fresnel(ray.direction(), intersect._normal, kr);
				if (kr < 1) {
					Ray refract_ray = intersect._material.get_refracted_ray(ray, ray.origin() + intersect._u*ray.direction(), intersect._normal, Xi);
					refract_color = trace_ray(refract_ray, depth + 1, Xi);
				}
				return reflect_color*kr + refract_color*(1 - kr);
			}
			else {
				return reflect_color;
			}
			
		}
		else {
			return Vector3d(0, 0, 0);
		}
	}

	Vector3d Scene::shadowRay(const Ray & ray,ObjectIntersection intersect,int hit_id)
	{
		Vector3d color(0, 0, 0);
		 double t;
		Vector3d hitPos = ray.origin() + ray.direction()*intersect._u;
		for (int j = 0; j < lightSamples.size(); j++) {
			bool inShadow = false;Vector3d lightPos = lightSamples[j];
			Vector3d direction = (lightPos - hitPos).normalized();
			Ray shadowRay = Ray(hitPos, direction);
			for (int i = 1; i < _objects.size(); i++) {
				if (i == hit_id || i == 3) { continue; }
				if (_objects[i]->box.intersection(shadowRay, t)) {
					ObjectIntersection tmp = _objects[i]->getIntersection(shadowRay);
					if (tmp._hit&&tmp._u > 0 && tmp._u < (lightPos - hitPos).norm()) {
						inShadow = true;
						break;
					}
				}
			}
			if (!inShadow) {
				double costheta = intersect._normal.normalized().dot(direction);
				color += intersect._material.get_colour()*/*Vector3d(25.0f, 25.0f, 25.0f)**/costheta;
			}
		}
		return color/lightSamples.size();
	}



}