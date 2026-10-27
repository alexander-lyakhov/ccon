#ifndef CUBE_H__
#define CUBE_H__

#include <stdint.h>
#include <unistd.h>

#include "./plane.h"

#define ORIGIN_IMPLEMENTATION
#include "./origin.h"

#define FACE_COUNT 6
#define PI 3.14159

typedef struct Cube {
	Origin  origin, origin_default;
	Plane   face[FACE_COUNT];
	Plane*  current_face;
	char*   texture;
	size_t  texture_chunks;
	size_t  texture_width;
	
	size_t  sides;
	uint8_t curr_face_index;

	uint8_t is_rotating;
	uint8_t is_texture_visible;
} Cube;

// Function prototypes
//----------------------------------------------------------------------------------
Cube    Cube_create                 (Vec3D v);
void    Cube_init                   (Cube *cube, Vec3D v);
char*   Cube_load_texture           (Cube *cube, const char *filename);
void    Cube_mem_free               (Cube *cube);
void    Cube_translate_x            (Cube *cube, float dx);
void    Cube_translate_y            (Cube *cube, float dy);
void    Cube_translate_z            (Cube *cube, float dz);
void    Cube_rotate_x               (Cube *cube, float angle);
void    Cube_rotate_y               (Cube *cube, float angle);
void    Cube_rotate_z               (Cube *cube, float angle);
void    Cube_toggle_rotate          (Cube *cube);
void    Cube_reset                  (Cube *cube);
Plane*  Cube_set_current_face       (Cube *cube, uint8_t face_index);
Plane*  Cube_get_current_face       (Cube *cube);
uint8_t Cube_should_display_texture (Cube *cube);
Vec3D   Cube_calculate_single_point (Cube *cube, const Vec3D *v);
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
		cube->sides              = FACE_COUNT;
		cube->curr_face_index    = 0;
		cube->is_rotating        = 1;
		cube->is_texture_visible = 1;
		cube->texture            = NULL;
		cube->texture_chunks     = 0;
		cube->texture_width      = 0;

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

		cube->face[5].origin.rx = PI;
		cube->face[5].origin.ry = 0;
		cube->face[5].origin.rz = 0;
		
		/*
		cube->face[0].origin.rx = 0;
		cube->face[0].origin.ry = 0;
		cube->face[0].origin.rz = 0;

		cube->face[1].origin.rx = 0;
		cube->face[1].origin.ry = PI / 2;
		cube->face[1].origin.rz = 0;

		cube->face[2].origin.rx = 0;
		cube->face[2].origin.ry = PI;
		cube->face[2].origin.rz = 0;

		cube->face[3].origin.rx = 0;
		cube->face[3].origin.ry = -PI / 2;
		cube->face[3].origin.rz = 0;

		cube->face[4].origin.rx = -PI / 2;
		cube->face[4].origin.ry = 0;
		cube->face[4].origin.rz = 0;

		cube->face[5].origin.rx = PI / 2;
		cube->face[5].origin.ry = 0;
		cube->face[5].origin.rz = 0;
		*/

		char texture[] = "+;i%#$";
		// char texture[] = "$#%i;+";
		// char texture[] = "%#$%#$";

		for (int i = 0; i < FACE_COUNT; i++)
			Plane_init(&(cube->face[i]), (Vec3D){0, 0, 0}, texture[i]);

		cube->origin_default = cube->origin;
	}

	// =============================================================================
	// @@@ + Cube_load_texture
	// =============================================================================
	char* Cube_load_texture(Cube *cube, const char *filename)
	{
		FILE *file = fopen(filename, "r");

		if (!file) return NULL;

		char str[1024];
		size_t texture_width = 0;
		
		for (char *p = NULL;;)
		{
			fgets(str, 1024, file);

			if (feof(file))
				break;

			if (cube->texture == NULL)
			{
				texture_width = strlen(str) - 1; // -1 is the new line character
				cube->texture_chunks = texture_width / SIZE;
				cube->texture = malloc(sizeof(char) * texture_width * SIZE);
				p = cube->texture;
			}

			memcpy(p, str, texture_width);
			p += texture_width;

			memcpy(p, str, texture_width);
			p += texture_width;
		}

		cube->texture_width = texture_width;

		return cube->texture;
	}

	// =============================================================================
	// @@@ + Cube_mem_free
	// =============================================================================
	void Cube_mem_free(Cube *cube)
	{
		Plane_mem_free(&(cube->face[0]));

		if (cube->texture)
			free(cube->texture);

		cube->texture = NULL;
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
		cube->origin.y += dy;
	}

	// =============================================================================
	// @@@ + Cube_translate_z
	// =============================================================================
	void Cube_translate_z(Cube *cube, float dz)
	{
		cube->origin.z += dz;
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
	// @@@ + Cube_toggle_rotate
	// =============================================================================
	void Cube_toggle_rotate(Cube *cube)
	{
		cube->is_rotating ^= 1;
	}

	// =============================================================================
	// @@@ + Cube_reset
	// =============================================================================
	void Cube_reset(Cube *cube)
	{
		cube->origin = cube->origin_default;
	}

	// =============================================================================
	// @@@ + Cube_set_current_face
	// =============================================================================
	Plane* Cube_set_current_face(Cube *cube, uint8_t face_index)
	{
		cube->curr_face_index = face_index;
		cube->current_face = &cube->face[face_index];
		
		return cube->current_face;
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
	// @@@ + Cube_should_display_texture
	// =============================================================================
	uint8_t Cube_should_display_texture(Cube *cube)
	{
		return cube->texture && cube->is_texture_visible;
	}

	// =============================================================================
	// @@@ + Cube_calculate_single_point
	// =============================================================================
	Vec3D Cube_calculate_single_point(Cube *cube, const Vec3D *v)
	{
		Vec3D p = *v;

		p = Vec3D_rotate_x(&p, cube->current_face->origin.rx);
		p = Vec3D_rotate_y(&p, cube->current_face->origin.ry);
		p = Vec3D_rotate_z(&p, cube->current_face->origin.rz);

		p = Vec3D_rotate_x(&p, cube->origin.rx);
		p = Vec3D_rotate_y(&p, cube->origin.ry);
		p = Vec3D_rotate_z(&p, cube->origin.rz);

		p.x += cube->origin.x + cube->current_face->origin.x;
		p.y += cube->origin.y + cube->current_face->origin.y;
		p.z += cube->origin.z + cube->current_face->origin.z;

		return p;
	}

#endif
#endif
