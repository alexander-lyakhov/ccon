#ifndef CAMERA_H__
#define CAMERA_H__

#define ORIGIN_IMPLEMENTATION
#include "./origin.h"

typedef struct Camera {
	Origin origin;
} Camera;

void Camera_init(Camera *camera, Vec3D v)
{
	camera->origin.x = v.x;
	camera->origin.y = v.y;
	camera->origin.z = v.z;
}

#endif
