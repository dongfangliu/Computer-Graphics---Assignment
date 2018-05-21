

#include "cuda_runtime.h"
#include "device_launch_parameters.h"
__global__ void renderRay(int samples, Camera * _camera, Vector3d * _pixelBuffer,Scene * _scene) {
	int x = blockIdx.x; int y = blockIdx.y; int a = threadIdx.x; int width = _camera->imageW();	int height = _camera->imageH();
	fprintf(stderr, "\rRendering (%i samples): %.2f%% ", samples, (double)blockIdx.y / height * 100);
	unsigned short Xi[3] = { 0,0,y*y*y };
	Ray ray = _camera->get_ray(blockIdx.x, blockIdx.y, a> 0, Xi);
	_pixelBuffer[(y)*width + x] += _scene->trace_ray(ray, 0, Xi);
}