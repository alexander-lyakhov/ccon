#ifndef VEC_H__
#define VEC_H__

typedef struct Vec2D {
	float x;
	float y;
} Vec2D;

typedef struct Vec3D {
	float x;
	float y;
	float z;
} Vec3D;

Vec3D Vec3D_rotate_x(Vec3D *p, float angle);
Vec3D Vec3D_rotate_y(Vec3D *p, float angle);
Vec3D Vec3D_rotate_z(Vec3D *p, float angle);

#ifdef VEC3D_IMPLEMENTATION

	// =============================================================================
	// @@@ + Vec3D_rotate_x
	// =============================================================================
	Vec3D Vec3D_rotate_x(Vec3D *p, float angle)
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
	// @@@ + Vec3D_rotate_y
	// =============================================================================
	Vec3D Vec3D_rotate_y(Vec3D *p, float angle)
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
	// @@@ + Vec3D_rotate_z
	// =============================================================================
	Vec3D Vec3D_rotate_z(Vec3D *p, float angle)
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
