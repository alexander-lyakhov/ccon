#ifndef CUBE_H__
#define CUBE_H__

#define ORIGIN_IMPLEMENTATION
#include "./origin.h"

#define FACE_COUNT 2
#define PI 3.14159

typedef struct Cube {
	Origin origin;
	Plane face[FACE_COUNT];
	size_t size;
} Cube;

#ifdef CUBE_IMPLEMENTATION

#define PLANE_IMPLEMENTATION
#include "./plane.h"

Cube Cube_create(Vec3D v);
void Cube_init(Cube *cube, Vec3D v);

// =============================================================================
// @@@ + Cube_create
// =============================================================================
Cube Cube_create(Vec3D v)
{
	Cube cube;
	Cube_init(&cube, v);

	return cube;
}

// =============================================================================
// @@@ + Cube_init
// =============================================================================
void Cube_init(Cube *cube, Vec3D v)
{
	Plane_init_static_buff();

	cube->origin.x = v.x;
	cube->origin.y = v.y;
	cube->origin.z = v.z;

	cube->size = FACE_COUNT;

	cube->face[0].origin.ry = PI / 2;
	cube->face[1].origin.ry = -PI / 2;
	// cube->face[2].origin.rx = PI / 2;
	
	Plane_init(&(cube->face[0]), (Vec3D){0, 0, 0}, '$');
	Plane_init(&(cube->face[1]), (Vec3D){0, 0, 0}, '#');
	// Plane_init(&(cube->face[2]), (Vec3D){0, 0, 0}, '@');
}

#endif
#endif
