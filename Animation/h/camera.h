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
// @@@ + plane_translate
// =============================================================================
void Camera_translate_x(Camera *camera, float dx)
{
	camera->origin.x += dx;
}

void Camera_translate_y(Camera *camera, float dy)
{
	camera->origin.y += dy;
}

void Camera_translate_z(Camera *camera, float dz)
{
	camera->origin.x -= dz * sin(camera->origin.ry);
	camera->origin.z += dz * cos(camera->origin.ry);
}

// =============================================================================
// @@@ + Camera_rotate
// =============================================================================
Vec3D Camera_rotate_x(Camera *camera, const Vec3D *p)
{
	return origin_rotate_y(p, camera->origin.rx);
}

Vec3D Camera_rotate_y(Camera *camera, const Vec3D *p)
{
	return origin_rotate_y(p, camera->origin.ry);
}

Vec3D Camera_rotate_z(Camera *camera, const Vec3D *p)
{
	return origin_rotate_y(p, camera->origin.rz);
}

// =============================================================================
// @@@ + Camera_calculate_single_point
// =============================================================================
Vec3D Camera_calculate_single_point(Camera *camera, const Vec3D *v)
{
	Vec3D p = *v;

	p.x +=  camera->origin.x;
	p.y +=  camera->origin.y;
	p.z += -camera->origin.z;

	p = Camera_rotate_x(camera, &p);
	p = Camera_rotate_y(camera, &p);
	p = Camera_rotate_z(camera, &p);

	return p;
}

#endif
#endif
