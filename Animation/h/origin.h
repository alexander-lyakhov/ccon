#ifndef ORIGIN_H__
#define ORIGIN_H__

#include "./vec.h"

typedef struct Origin {
	float x;
	float y;
	float z;

	float tx;
	float ty;
	float tz;

	float rx;
	float ry;
	float rz;

} Origin;

Vec3D origin_rotate_x(const Vec3D *p, float angle);
Vec3D origin_rotate_y(const Vec3D *p, float angle);
Vec3D origin_rotate_z(const Vec3D *p, float angle);

#ifdef ORIGIN_IMPLEMENTATION

// =============================================================================
// @@@ + origin_rotate_x
// =============================================================================
Vec3D origin_rotate_x(const Vec3D *p, float angle)
{
	float COS = cos(angle);
	float SIN = sin(angle);

	return (Vec3D) {
		p->x,
		p->y * COS - p->z * SIN,
		p->y * SIN + p->z * COS,
	};
}

// =============================================================================
// @@@ + origin_rotate_y
// =============================================================================
Vec3D origin_rotate_y(const Vec3D *p, float angle)
{
	float COS = cos(angle);
	float SIN = sin(angle);

	return (Vec3D) {
		p->x * COS - p->z * SIN,
		p->y,
		p->x * SIN + p->z * COS,
	};
}

// =============================================================================
// @@@ + origin_rotate_z
// =============================================================================
Vec3D origin_rotate_z(const Vec3D *p, float angle)
{
	float COS = cos(angle);
	float SIN = sin(angle);

	return (Vec3D) {
		p->x * COS - p->y * SIN,
		p->x * SIN + p->y * COS,
		p->z,
	};
}

#endif
#endif
