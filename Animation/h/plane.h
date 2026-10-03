#ifndef PLANE_H__
#define PLANE_H__

#define ORIGIN_IMPLEMENTATION
#include "./origin.h"

typedef struct Plane {
	Origin origin;
	Vec3D *buff;
} Plane;

void plane_translate_x(Plane *plane, float dx);
void plane_translate_y(Plane *plane, float dy);
void plane_translate_z(Plane *plane, float dz);

Vec3D plane_rotate_x(Plane *plane, Vec3D p);
Vec3D plane_rotate_y(Plane *plane, Vec3D p);
Vec3D plane_rotate_z(Plane *plane, Vec3D p);

#ifdef PLANE_IMPLEMENTATION

// =============================================================================
// @@@ + plane_translate_x
// =============================================================================
void plane_translate_x(Plane *plane, float dx)
{
	plane->origin.x += dx;
}
// =============================================================================
// @@@ + plane_translate_y
// =============================================================================
void plane_translate_y(Plane *plane, float dy)
{
	plane->origin.y += dy;
}

// =============================================================================
// @@@ + plane_translate_z
// =============================================================================
void plane_translate_z(Plane *plane, float dz)
{
	plane->origin.z += dz;
}

// =============================================================================
// @@@ + plane_rotate_x
// =============================================================================
Vec3D plane_rotate_x(Plane *plane, Vec3D p)
{
	return origin_rotate_x(&p, plane->origin.rx);
}

// =============================================================================
// @@@ + plane_rotate_y
// =============================================================================
Vec3D plane_rotate_y(Plane *plane, Vec3D p)
{
	return origin_rotate_y(&p, plane->origin.ry);
}

// =============================================================================
// @@@ + plane_rotate_z
// =============================================================================
Vec3D plane_rotate_z(Plane *plane, Vec3D p)
{
	return origin_rotate_z(&p, plane->origin.rz);
}

#endif
#endif
