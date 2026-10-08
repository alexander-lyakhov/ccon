#ifndef CUBE_H__
#define CUBE_H__

#define ORIGIN_IMPLEMENTATION
#include "./origin.h"

#define FACE_COUNT 1

typedef struct Cube {
	Origin origin;
	Plane face[FACE_COUNT];
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

	Plane_init(&(cube->face[0]), (Vec3D){0, 0, 0}, '$');
}

#endif
#endif
