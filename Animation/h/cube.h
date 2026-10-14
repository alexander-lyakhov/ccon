#ifndef CUBE_H__
#define CUBE_H__

#define ORIGIN_IMPLEMENTATION
#include "./origin.h"

#define FACE_COUNT 6
#define PI 3.14159

typedef struct Cube {
	Origin  origin;
	Plane   face[FACE_COUNT];
	size_t  size;
	uint8_t curr_face_index;
} Cube;

// Function prototypes
//----------------------------------------------------------------------------------
Cube   Cube_create                 (Vec3D v);
void   Cube_init                   (Cube *cube, Vec3D v);
void   Cube_translate_x            (Cube *cube, float dx);
void   Cube_translate_y            (Cube *cube, float dy);
void   Cube_translate_z            (Cube *cube, float dz);
void   Cube_rotate                 (Cube *cube, float rx, float ry, float rz);
void   Cube_rotate_x               (Cube *cube, float angle);
void   Cube_rotate_y               (Cube *cube, float angle);
void   Cube_rotate_z               (Cube *cube, float angle);
Plane* Cube_set_current_face       (Cube *cube, uint8_t face_index);
Plane* Cube_get_current_face       (Cube *cube);
Vec3D  Cube_calculate_single_point (Cube *cube, const Vec3D *v);
//----------------------------------------------------------------------------------
// End of function prototypes

#ifdef CUBE_IMPLEMENTATION

	#define PLANE_IMPLEMENTATION
	#include "./plane.h"

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

		cube->origin.rx = 0;
		cube->origin.ry = 0;
		cube->origin.rz = 0;

		cube->face[0].origin.rx = 0;
		cube->face[0].origin.ry = PI / 2;
		cube->face[0].origin.rz = 0;

		cube->face[1].origin.rx = 0;
		cube->face[1].origin.ry = -PI / 2;
		cube->face[1].origin.rz = 0;

		cube->face[2].origin.rx = PI / 2;
		cube->face[2].origin.ry = 0;
		cube->face[2].origin.rz = 0;

		cube->face[3].origin.rx = -PI / 2;
		cube->face[3].origin.ry = 0;
		cube->face[3].origin.rz = 0;

		cube->face[4].origin.rx = 0;
		cube->face[4].origin.ry = 0;
		cube->face[4].origin.rz = 0;

		cube->face[5].origin.rx = 0;
		cube->face[5].origin.ry = PI;
		cube->face[5].origin.rz = 0;
		
		Plane_init(&(cube->face[0]), (Vec3D){0, 0, 0}, '.');
		Plane_init(&(cube->face[1]), (Vec3D){0, 0, 0}, ';');
		Plane_init(&(cube->face[2]), (Vec3D){0, 0, 0}, 'i');
		Plane_init(&(cube->face[3]), (Vec3D){0, 0, 0}, '*');
		Plane_init(&(cube->face[4]), (Vec3D){0, 0, 0}, '+');
		Plane_init(&(cube->face[5]), (Vec3D){0, 0, 0}, 'i');
		
		/*Plane_init(&(cube->face[0]), (Vec3D){0, 0, 0}, '1');
		Plane_init(&(cube->face[1]), (Vec3D){0, 0, 0}, '2');
		Plane_init(&(cube->face[2]), (Vec3D){0, 0, 0}, '3');
		Plane_init(&(cube->face[3]), (Vec3D){0, 0, 0}, '4');
		Plane_init(&(cube->face[4]), (Vec3D){0, 0, 0}, '5');
		Plane_init(&(cube->face[5]), (Vec3D){0, 0, 0}, '6');*/

		cube->size = FACE_COUNT;
		cube->curr_face_index = 0;
	}

	// =============================================================================
	// @@@ + Cube_translate_x
	// =============================================================================
	void Cube_translate_x(Cube *cube, float dx)
	{
		cube->origin.x += dx;
	}

	// =============================================================================
	// @@@ + Cube_translate_y
	// =============================================================================
	void Cube_translate_y(Cube *cube, float dy)
	{
		cube->origin.x += dy;
	}

	// =============================================================================
	// @@@ + Cube_translate_z
	// =============================================================================
	void Cube_translate_z(Cube *cube, float dz)
	{
		cube->origin.x += dz;
	}

	// =============================================================================
	// @@@ + Cube_rotate
	// =============================================================================
	void Cube_rotate(Cube *cube, float rx, float ry, float rz)
	{
		cube->origin.rx += rx;
		cube->origin.ry += ry;
		cube->origin.rz += rz;
	}

	// =============================================================================
	// @@@ + Cube_rotate_x
	// =============================================================================
	void Cube_rotate_x(Cube *cube, float angle)
	{
		cube->origin.rx += angle;
	}

	// =============================================================================
	// @@@ + Cube_rotate_y
	// =============================================================================
	void Cube_rotate_y(Cube *cube, float angle)
	{
		cube->origin.ry += angle;
	}

	// =============================================================================
	// @@@ + Cube_rotate_z
	// =============================================================================
	void Cube_rotate_z(Cube *cube, float angle)
	{
		cube->origin.rz += angle;
	}

	// =============================================================================
	// @@@ + Cube_set_current_face
	// =============================================================================
	Plane* Cube_set_current_face(Cube *cube, uint8_t face_index)
	{
		cube->curr_face_index = face_index;
		return &cube->face[face_index];
	}

	// =============================================================================
	// @@@ + Cube_get_current_face
	// =============================================================================
	Plane* Cube_get_current_face(Cube *cube)
	{
		return &cube->face[
			cube->curr_face_index
		];
	}

	// =============================================================================
	// @@@ + Cube_calculate_single_point
	// =============================================================================
	Vec3D Cube_calculate_single_point(Cube *cube, const Vec3D *v)
	{
		Plane *face = &cube->face[
			cube->curr_face_index
		];

		Vec3D p = *v;

		p = Vec3D_rotate_x(&p, face->origin.rx);
		p = Vec3D_rotate_y(&p, face->origin.ry);
		p = Vec3D_rotate_z(&p, face->origin.rz);

		p = Vec3D_rotate_x(&p, cube->origin.rx);
		p = Vec3D_rotate_y(&p, cube->origin.ry);
		p = Vec3D_rotate_z(&p, cube->origin.rz);

		p.x += cube->origin.x + face->origin.x;
		p.y += cube->origin.y + face->origin.y;
		p.z += cube->origin.z + face->origin.z;

		return p;
	}

#endif
#endif
