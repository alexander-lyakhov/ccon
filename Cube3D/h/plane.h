#ifndef PLANE_H__
#define PLANE_H__

#include <unistd.h>
#include "./vec.h"

#define ORIGIN_IMPLEMENTATION
#include "./origin.h"

typedef struct Plane {
	Origin origin;
	Vec3D* buff;
	size_t buff_size;
	char   ascii_char;

} Plane;

// Function prototypes
//----------------------------------------------------------------------------------
void  Plane_init_static_buff();
void  Plane_mem_free    (Plane *plane);

Plane Plane_create      (Vec3D pos, Vec3D t, Vec3D r, char ascii_char);
void  Plane_init        (Plane *plane, Vec3D pos, Vec3D t, Vec3D r, char ascii_char);

void  Plane_translate_x (Plane *plane, float dx);
void  Plane_translate_y (Plane *plane, float dy);
void  Plane_translate_z (Plane *plane, float dz);

void  Plane_rotate_x    (Plane *plane, float angle);
void  Plane_rotate_y    (Plane *plane, float angle);
void  Plane_rotate_z    (Plane *plane, float angle);
//----------------------------------------------------------------------------------
// End of function prototypes

#ifdef PLANE_IMPLEMENTATION

	#define SIZE 80
	static Vec3D *buff;

	// =============================================================================
	// @@@ + Plane_init_static_buff
	// =============================================================================
	void Plane_init_static_buff()
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
					.z = 0
				};
			}
		}
	}

	// =============================================================================
	// @@@ + Plane_mem_free
	// =============================================================================
	void Plane_mem_free(Plane *plane)
	{
		if (plane->buff)
			free(plane->buff);

		plane->buff = NULL;
	}

	// =============================================================================
	// @@@ + Plane_create
	// =============================================================================
	Plane Plane_create(Vec3D pos, Vec3D t, Vec3D r, char ascii_char)
	{
		Plane plane;
		Plane_init(&plane, pos, t, r, ascii_char);

		return plane;
	}

	// =============================================================================
	// @@@ + Plane_init
	// =============================================================================
	void Plane_init(Plane *plane, Vec3D pos, Vec3D t, Vec3D r, char ascii_char)
	{
		plane->origin.x   = pos.x;
		plane->origin.y   = pos.y;
		plane->origin.z   = pos.z;
		
		plane->origin.tx  = t.x;
		plane->origin.ty  = t.y;
		plane->origin.tz  = t.z;

		plane->origin.rx  = r.x;
		plane->origin.ry  = r.y;
		plane->origin.rz  = r.z;

		plane->ascii_char = ascii_char;
		plane->buff       = buff;
		plane->buff_size  = SIZE * SIZE;
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
	void Plane_rotate_x(Plane *plane, float angle)
	{
		plane->origin.rx += angle;
	}

	// =============================================================================
	// @@@ + Plane_rotate_y
	// =============================================================================
	void Plane_rotate_y(Plane *plane, float angle)
	{
		plane->origin.ry += angle;
	}

	// =============================================================================
	// @@@ + Plane_rotate_z
	// =============================================================================
	void Plane_rotate_z(Plane *plane, float angle)
	{
		plane->origin.rz += angle;
	}

#endif
#endif
