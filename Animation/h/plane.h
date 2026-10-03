#ifndef PLANE_H__
#define PLANE_H__

#define ORIGIN_IMPLEMENTATION
#include "./origin.h"

typedef struct Plane {
	Origin origin;
	Vec3D *buff;
} Plane;

void Plane_translate_x(Plane *plane, float dx);
void Plane_translate_y(Plane *plane, float dy);
void Plane_translate_z(Plane *plane, float dz);

Vec3D Plane_rotate_x(Plane *plane, Vec3D p);
Vec3D Plane_rotate_y(Plane *plane, Vec3D p);
Vec3D Plane_rotate_z(Plane *plane, Vec3D p);

#ifdef PLANE_IMPLEMENTATION

// =============================================================================
// @@@ + Plane_translate_x
// =============================================================================
void Plane_translate_x(Plane *plane, float dx)
{
	plane->origin.x += dx;
}
// =============================================================================
// @@@ + Plane_translate_y
// =============================================================================
void Plane_translate_y(Plane *plane, float dy)
{
	plane->origin.y += dy;
}

// =============================================================================
// @@@ + Plane_translate_z
// =============================================================================
void Plane_translate_z(Plane *plane, float dz)
{
	plane->origin.z += dz;
}

// =============================================================================
// @@@ + Plane_rotate_x
// =============================================================================
Vec3D Plane_rotate_x(Plane *plane, Vec3D p)
{
	return origin_rotate_x(&p, plane->origin.rx);
}

// =============================================================================
// @@@ + Plane_rotate_y
// =============================================================================
Vec3D Plane_rotate_y(Plane *plane, Vec3D p)
{
	return origin_rotate_y(&p, plane->origin.ry);
}

// =============================================================================
// @@@ + Plane_rotate_z
// =============================================================================
Vec3D Plane_rotate_z(Plane *plane, Vec3D p)
{
	return origin_rotate_z(&p, plane->origin.rz);
}

#endif
#endif
