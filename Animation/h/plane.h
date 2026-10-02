#ifndef PLANE_H__
#define PLANE_H__

#include "./vec.h"
#include "./transform.h"

typedef struct Plane {
	Transform origin;
	Vec3D *buff;
} Plane;

void plane_translate_z(Plane *plane, float dz);

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
// @@@ + rotate_x
// =============================================================================
Vec3D rotate_x(Plane *plane, Vec3D p)
{
	float angle = plane->origin.ry;

	return (Vec3D) {
		p.x,
		p.y * cos(angle) - p.z * sin(angle),
		p.y * sin(angle) + p.z * cos(angle)
	};
}

// =============================================================================
// @@@ + rotate_y
// =============================================================================
Vec3D rotate_y(Plane *plane, Vec3D p)
{
	float angle = plane->origin.ry;

	return (Vec3D) {
		p.x * cos(angle) - p.z * sin(angle),
		p.y,
		p.x * sin(angle) + p.z * cos(angle)
	};
}

// =============================================================================
// @@@ + rotate_z
// =============================================================================
Vec3D rotate_z(Plane *plane, Vec3D p)
{
	float angle = plane->origin.rz;

	return (Vec3D) {
		p.x * cos(angle) - p.y * sin(angle),
		p.x * sin(angle) + p.y * cos(angle),
		p.z,
	};
}

#endif
#endif
