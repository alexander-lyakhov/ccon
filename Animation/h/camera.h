#ifndef CAMERA_H__
#define CAMERA_H__

#include "./vec.h"

#define ORIGIN_IMPLEMENTATION
#include "./origin.h"

typedef struct Camera {
	Origin origin;
} Camera;

Camera Camera_create(Vec3D v);
void   Camera_init(Camera *camera, Vec3D v);
void   Camera_translate_x(Camera *camera, float dx);
void   Camera_translate_y(Camera *camera, float dx);
void   Camera_translate_z(Camera *camera, float dx);

#ifdef CAMERA_IMPLEMENTATION

// =============================================================================
// @@@ + Camera_create
// =============================================================================
Camera Camera_create(Vec3D v)
{
	Camera camera;
	Camera_init(&camera, v);
	return camera;
}

// =============================================================================
// @@@ + Camera_init
// =============================================================================
void Camera_init(Camera *camera, Vec3D v)
{
	camera->origin.x = v.x;
	camera->origin.y = v.y;
	camera->origin.z = v.z;
}

// =============================================================================
// @@@ + plane_translate_x
// =============================================================================
void Camera_translate_x(Camera *camera, float dx)
{
	camera->origin.x += dx;
}
// =============================================================================
// @@@ + plane_translate_y
// =============================================================================
void Camera_translate_y(Camera *camera, float dy)
{
	camera->origin.y += dy;
}

// =============================================================================
// @@@ + plane_translate_z
// =============================================================================
void Camera_translate_z(Camera *camera, float dz)
{
	camera->origin.z += dz;
}

#endif
#endif
