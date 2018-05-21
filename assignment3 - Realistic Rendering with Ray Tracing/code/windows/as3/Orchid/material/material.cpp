#include "material.h"
#include "../erand48.h"
#include "../sample/sampler.h"
namespace Orchid
{
	const float ior = 1.5;
	Material::Material
	(MaterialType t, Vector3d colour, Vector3d emission, Texture texture)
		:_type{ t }
		, _colour{ colour }
		, _emission{ emission }
		, _texture{ texture }  //nead addd
	{

	}

	Material & Material::operator=(Material  m)
	{
		this->_type = m._type;
		this->_emission = m._emission;
		this->_texture = m._texture;
		this->_colour = m._colour;
		return *this;
	}

	Vector3d Material::get_colour_at(double u, double v) const
	{
		if (_texture.is_loaded())
			return _texture.get_pixel(u, v);
		return _colour;
	}

	Ray Material::get_reflected_ray
	(
		const Ray & r, 
		Vector3d & p, 
		const Vector3d & n,
		unsigned short * Xi
	) const
	{
		float decrease = 1;
		Vector3d reflect_direct;
		if(_type == MaterialType::DIFF||_type==MaterialType::REFRC){
			reflect_direct = Vector3d(0, 0, 0);
			double r1 = erand48(Xi); double r2 = erand48(Xi);
			sampler::onHemisphere(n, &reflect_direct,r1, r2,&decrease);
			reflect_direct =reflect_direct.normalized();
		}else{
			reflect_direct = Vector3d::reflect(r.direction(), n);
			reflect_direct = reflect_direct.normalized();
			decrease = n.normalized().dot(reflect_direct);
		}
		Ray a = Ray(p, reflect_direct);
		a.attenuation = decrease;
		return a;
	}
	Ray Material::get_refracted_ray
	(
		const Ray & r,
		Vector3d & p,
		const Vector3d & n,
		unsigned short * Xi
	) const
	{
		float decrease = 1;
		Vector3d reflect_direct;
		float cosi = clamp(-1.0, 1.0, r.direction().dot(n));
		float etai = 1, etat = ior;
		Vector3d _normal = n;
		if (cosi < 0) {
			cosi = -cosi;
		}
		else {
			std::swap(etai, etat);
			_normal = -n;
		}
		float eta = etai / etat;
		float k = 1 - eta * eta * (1 - cosi * cosi);
		if (k < 0) {
			reflect_direct = Vector3d(0, 0, 0);
		}
		else {
			reflect_direct = r.direction()*eta + _normal*(eta * cosi - sqrtf(k));
		}
		reflect_direct = reflect_direct.normalized();
		decrease = n.normalized().dot(reflect_direct);
		Ray a = Ray(p, reflect_direct);
		a.attenuation = decrease;
		return a;
	}
	void Material::fresnel(const Vector3d & I, const Vector3d & N, float & kr)
	{
		float cosi = clamp(-1.0, 1.0, I.dot(N));
		float etai = 1, etat = ior;
		if (cosi > 0) { std::swap(etai, etat); }
		// Compute sini using Snell's law
		float sint = etai / etat * sqrtf(std::max(0.f, 1 - cosi * cosi));
		// Total internal reflection
		if (sint >= 1) {
			kr = 1;
		}
		else {
			float cost = sqrtf(std::max(0.f, 1 - sint * sint));
			cosi = fabsf(cosi);
			float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
			float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
			kr = (Rs * Rs + Rp * Rp) / 2;
		}
		// As a consequence of the conservation of energy, transmittance is given by:
		// kt = 1 - kr;
	}
}