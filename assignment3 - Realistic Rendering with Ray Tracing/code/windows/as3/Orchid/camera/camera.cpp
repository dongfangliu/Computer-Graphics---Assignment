#include "camera.h"
#include <random>
#include "../erand48.h"
namespace Orchid
{
	Camera::Camera
	(
		Vector3d position, Vector3d target, Vector3d up,
		int width, int height,
		double nearPalneDistance, double fov
	) :	_position{ position },
		_up{ up },
		_imageW{ width },
		_imageH{ height },
		_nearPlaneDistance{ nearPalneDistance }
	{
		_cameraVerticalFOV = fov *PI / 180.0f;
		_aspectRatio = (double)_imageW / _imageH;
		_cameraFwd = (target - _position).normalized();
		_cameraRight = _up.cross(_cameraFwd).normalized();
		_cameraUp = _cameraFwd.cross(_cameraRight).normalized()*(-1);///???
		_cameraHorizFOV = _cameraVerticalFOV *_aspectRatio;
		_windowTop = tan(_cameraVerticalFOV / 2.0f)*_nearPlaneDistance;
		_windowRight = tan(_cameraHorizFOV / 2.0f)*_nearPlaneDistance;
	}

	Ray Camera::get_ray(int x, int y, bool jitter, unsigned short * Xi)
	{
		double x_new = x, y_new = y;
		if (jitter)
		{
			x_new += erand48(Xi);
			y_new += erand48(Xi);
		}
		x_new = 2 * ((x_new + 0.5) / _imageW) - 1;
		y_new =  2 * ((y_new + 0.5) / _imageH)-1;
		
		Vector3d Px = _cameraRight *x_new * tan(_cameraHorizFOV / 2  )* _aspectRatio *_nearPlaneDistance;
		Vector3d Py = _cameraUp*y_new * tan(_cameraVerticalFOV / 2 ) *_nearPlaneDistance;
		Vector3d Pz = _nearPlaneDistance*_cameraFwd;

		Vector3d rayDirection =Px+Py+ Pz;

		rayDirection = rayDirection.normalized(); // it's a direction so don't forget to normalize 
		return Ray(_position, rayDirection);

	}

}
