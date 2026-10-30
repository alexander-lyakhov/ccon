#ifndef CUBE_H__
#define CUBE_H__

#include <stdint.h>
#include <unistd.h>

#include "./plane.h"

#define ORIGIN_IMPLEMENTATION
#include "./origin.h"

#define TEXTURE_IMPLEMENTATION
#include "./texture.h"

#define FACE_COUNT 6
#define PI 3.14159

typedef struct Cube {
	Origin  origin, origin_default;
	Texture texture;
	Plane   face[FACE_COUNT];
	Plane*  current_face;
	
	size_t  sides;
	uint8_t curr_face_index;

	uint8_t is_rotating;
	uint8_t is_expanded;
	uint8_t is_expanded_ones;
	uint8_t is_texture_visible;

	float expand_phase;
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
void    Cube_toggle_expand          (Cube *cube);
void    Cube_try_to_expand          (Cube *cube, Vec3D *p);
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
	    Texture_init(&cube->texture);

		cube->sides              = FACE_COUNT;
		cube->curr_face_index    = 0;
		cube->expand_phase       = 0;
		cube->is_rotating        = 1;
		cube->is_expanded        = 0;
		cube->is_expanded_ones   = 0;
		cube->is_texture_visible = 1;

		Plane_init_static_buff();

		cube->origin.x = v.x;
		cube->origin.y = v.y;
		cube->origin.z = v.z;

		cube->origin.rx = 0;
		cube->origin.ry = 0;
		cube->origin.rz = 0;

		char texture[] = "+;i%#$";
		// char texture[] = "$#%i;+";
		// char texture[] = "$#%%#$";

		for (int i = 0; i < FACE_COUNT; i++)
			Plane_init(
				&(cube->face[i]),
				(Vec3D){0, 0, 0}, // Position
				(Vec3D){0, 0, 1}, // Translation
				(Vec3D){0, 0, 0}, // Rotation
				texture[i]
			);
		
		cube->face[0].origin.ry = PI / 2;
		cube->face[1].origin.ry = -PI / 2;

		cube->face[2].origin.rx = PI / 2;
		cube->face[3].origin.rx = -PI / 2;
		cube->face[4].origin.rx = 0;
		cube->face[5].origin.rx = PI;
		
		/*
		cube->face[0].origin.ry = 0;
		cube->face[1].origin.ry = PI / 2;
		cube->face[2].origin.ry = PI;
		cube->face[3].origin.ry = -PI / 2;
		cube->face[4].origin.rx = -PI / 2;
		cube->face[5].origin.rx = PI / 2;
		*/

		cube->origin_default = cube->origin;
	}

	// =============================================================================
	// @@@ + Cube_load_texture
	// =============================================================================
	char* Cube_load_texture(Cube *cube, const char *filename)
	{
		Texture_load(&cube->texture, filename);
	}

	// =============================================================================
	// @@@ + Cube_mem_free
	// =============================================================================
	void Cube_mem_free(Cube *cube)
	{
		  Plane_mem_free(&cube->face[0]);
		Texture_mem_free(&cube->texture);
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
		return cube->texture.buff && cube->is_texture_visible;
	}

	// =============================================================================
	// @@@ + Cube_toggle_expand
	// =============================================================================
	void Cube_toggle_expand(Cube *cube)
	{
		cube->is_expanded ^= 1;
	    cube->is_expanded_ones = 1;
		cube->expand_phase = cube->origin.rz;
	}

	// =============================================================================
	// @@@ + Cube_try_explode
	// =============================================================================
	void Cube_try_to_expand(Cube *cube, Vec3D *p)
	{
	    
		if (cube->is_expanded)
			p->z += (cube->origin.rz - cube->expand_phase < PI / 2)
				? sin(cube->origin.rz - cube->expand_phase)
				: 1;

		if (!cube->is_expanded)
			p->z += (cube->origin.rz - cube->expand_phase < PI / 2 && cube->is_expanded_ones)
				? 1 - sin(cube->origin.rz - cube->expand_phase)
				: 0;
	}

	// =============================================================================
	// @@@ + Cube_calculate_single_point
	// =============================================================================
	Vec3D Cube_calculate_single_point(Cube *cube, const Vec3D *v)
	{
		Vec3D p = *v;
		
		p.x += cube->current_face->origin.tx;
		p.y += cube->current_face->origin.ty;
		p.z += cube->current_face->origin.tz;
		// p.z += cube->current_face->origin.tz + cube->is_exploding;

		Cube_try_to_expand(cube, &p); // may modify p.z

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
