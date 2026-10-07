#ifndef PLANE_H__
#define PLANE_H__

#define ORIGIN_IMPLEMENTATION
#include "./origin.h"

typedef struct Plane {
	Origin origin;
	Vec3D *buff;
	char texture;

} Plane;

// =============================================================================
// @@@ Prototypes
// =============================================================================
void  init_plane_buff();

Plane Plane_create(Vec3D v, char texture);
void  Plane_init(Plane *plane, Vec3D v, char texture);
void  Plane_free(Plane *plane);

void  Plane_translate_x(Plane *plane, float dx);
void  Plane_translate_y(Plane *plane, float dy);
void  Plane_translate_z(Plane *plane, float dz);

Vec3D Plane_rotate_x(Plane *plane, Vec3D p);
Vec3D Plane_rotate_y(Plane *plane, Vec3D p);
Vec3D Plane_rotate_z(Plane *plane, Vec3D p);

#ifdef PLANE_IMPLEMENTATION

#define SIZE 80
static Vec3D *buff;

// =============================================================================
// @@@ + init_static_plane_buff
// =============================================================================
void init_static_plane_buff()
{
	buff = malloc(SIZE * SIZE * sizeof(Vec3D));

	float kx_distribution = (float)SIZE / (SIZE - 1);
	float ky_distribution = (float)SIZE / (SIZE - 1);

	for (size_t row = 0, index = 0; row < SIZE; row++)
	{
		for (size_t col = 0; col < SIZE; col++)
		{
			buff[index++] = (Vec3D) {
				.x = (float)col / SIZE * kx_distribution * 2 - 1,
				.y = (float)row / SIZE * ky_distribution * 2 - 1,
				.z = 1
			};
		}
	}
}

// =============================================================================
// @@@ + Plane_create
// =============================================================================
Plane Plane_create(Vec3D v, char texture)
{
	Plane plane;
	Plane_init(&plane, v, texture);

	return plane;
}

// =============================================================================
// @@@ + Plane_init
// =============================================================================
void Plane_init(Plane *plane, Vec3D v, char texture)
{
	plane->origin.x = v.x;
	plane->origin.y = v.y;
	plane->origin.z = v.z;

	plane->texture = texture;
	plane->buff = buff;

	/*
	plane->buff = malloc(SIZE * SIZE * sizeof(Vec3D));

	float kx_distribution = (float)SIZE / (SIZE - 1);
	float ky_distribution = (float)SIZE / (SIZE - 1);

	for (size_t row = 0, index = 0; row < SIZE; row++)
	{
		for (size_t col = 0; col < SIZE; col++)
		{
			plane->buff[index++] = (Vec3D) {
				.x = (float)col / SIZE * kx_distribution * 2 - 1,
				.y = (float)row / SIZE * ky_distribution * 2 - 1,
				.z = offset_z,
			};
		}
	}
	*/
}

// =============================================================================
// @@@ + Plane_free
// =============================================================================
void Plane_free(Plane *plane)
{
	free(plane->buff);
	plane->buff = NULL;
}

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
