#ifndef CAMERA_H__
#define CAMERA_H__

#include "./vec.h"

#define ORIGIN_IMPLEMENTATION
#include "./origin.h"

typedef struct Camera {
	Origin origin;
} Camera;

// Function prototypes
//----------------------------------------------------------------------------------
Camera Camera_create                 (Vec3D v);
void   Camera_init                   (Camera *camera, Vec3D v);
void   Camera_translate_x            (Camera *camera, float dx);
void   Camera_translate_y            (Camera *camera, float dx);
void   Camera_translate_z            (Camera *camera, float dx);
Vec3D  Camera_rotate_x               (Camera *camera, float angle);
Vec3D  Camera_rotate_y               (Camera *camera, float angle);
Vec3D  Camera_rotate_z               (Camera *camera, float angle);
Vec3D  Camera_calculate_single_point (Camera *camera, const Vec3D *v);
//----------------------------------------------------------------------------------
// End of function prototypes

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
	// @@@ + Camera_translate_x
	// =============================================================================
	void Camera_translate_x(Camera *camera, float dx)
	{
		camera->origin.x += dx;
	}

	// =============================================================================
	// @@@ + Camera_translate_y
	// =============================================================================
	void Camera_translate_y(Camera *camera, float dy)
	{
		camera->origin.y += dy;
	}

	// =============================================================================
	// @@@ + Camera_translate_z
	// =============================================================================
	void Camera_translate_z(Camera *camera, float dz)
	{
		camera->origin.x -= dz * sin(camera->origin.ry);
		camera->origin.z += dz * cos(camera->origin.ry);
	}

	// =============================================================================
	// @@@ + Camera_rotate_x
	// =============================================================================
	Vec3D Camera_rotate_x(Camera *camera, float angle)
	{
		// return origin_rotate_y(p, camera->origin.rx);
		camera->origin.rx += angle;
	}

	// =============================================================================
	// @@@ + Camera_rotate_y
	// =============================================================================
	Vec3D Camera_rotate_y(Camera *camera, float angle)
	{
		// return origin_rotate_y(p, camera->origin.ry);
		camera->origin.ry += angle;
	}

	// =============================================================================
	// @@@ + Camera_rotate_z
	// =============================================================================
	Vec3D Camera_rotate_z(Camera *camera, float angle)
	{
		// return origin_rotate_y(p, camera->origin.rz);
		camera->origin.rz += angle;
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

		/*
		p = Camera_rotate_x(camera, &p);
		p = Camera_rotate_y(camera, &p);
		p = Camera_rotate_z(camera, &p);
		*/
		p = origin_rotate_x(&p, camera->origin.rx);
		p = origin_rotate_y(&p, camera->origin.ry);
		p = origin_rotate_z(&p, camera->origin.rz);
		return p;
	}

#endif
#endif
