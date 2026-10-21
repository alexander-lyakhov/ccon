/*
=============================
	Simple Graphic Engine
=============================
*/

#ifndef SGE_H__
#define SGE_H__

#include <stdbool.h>

#include "./vec.h"

typedef struct SGE {
	Console *console;
	float *z_buffer;

	float scale;
	float scale_x_corr;

} SGE;

/* 01 */	SGE   SGE_create           ();
/* 02 */	SGE*  SGE_init             (SGE *engine);
/* 03 */	void  SGE_mem_free         (SGE *engine);
/* 04 */    void  SGE_reset            (SGE *engine);
/* 05 */	Vec2D SGE_project_2d       (const Vec3D p);
/* 06 */	COORD SGE_point_to_screen  (SGE *engine, Vec2D p);
/* 07 */	void  SGE_reset_z_buffer   (SGE *engine);
/* 08 */	bool  SGE_check_screen_pos (SGE *engine, const COORD *screen_pos);

#ifdef SGE_IMPLEMENTATION

	// =============================================================================
	// @@@ + SGE_create
	// =============================================================================
	SGE SGE_create()
	{
		SGE engine;
		SGE_init(&engine);

		return engine;
	}

	// =============================================================================
	// @@@ + SGE_init
	// =============================================================================
	SGE* SGE_init(SGE *engine)
	{
		float screen_ar = (float)engine->console->width / engine->console->height;
		float font_ar   = engine->console->font_ar;

		if (!font_ar)
			font_ar = 0.462; // fallback for my Windows Terminal

		if (!engine->z_buffer)
			engine->z_buffer = malloc(sizeof(float) * engine->console->size),

		engine->scale_x_corr = engine->scale / (screen_ar * font_ar);

		return engine;
	}

	// =============================================================================
	// @@@ + SGE_free
	// =============================================================================
	void SGE_mem_free(SGE *engine)
	{
		if (engine->z_buffer)
			free(engine->z_buffer);

		engine->z_buffer = NULL;
	}

	// =============================================================================
	// @@@ + SGE_reset
	// =============================================================================
	void SGE_reset(SGE *engine)
	{
		SGE_mem_free(engine);
		SGE_init(engine);
	}

	// =============================================================================
	// @@@ + SGE_project_2d
	// =============================================================================
	Vec2D SGE_project_2d(const Vec3D p)
	{
		if (p.z <= 0)
			return (Vec2D){ .x = 1000, .y = 1000 }; // Return values that are too big so that the point isnt't drawn on the screen

		return (Vec2D) {
			.x = p.x / p.z,
			.y = p.y / p.z,
		};
	};

	// =============================================================================
	// @@@ + SGE_point_to_screen
	// =============================================================================
	COORD SGE_point_to_screen(SGE *engine, Vec2D p)
	{
		return (COORD) {
			round(engine->console->width  * (1 + p.x * engine->scale_x_corr) / 2),
			round(engine->console->height * (1 - p.y * engine->scale) / 2),
		};
	}

	// =============================================================================
	// @@@ + SGE_reset_z_buffer
	// =============================================================================
	void SGE_reset_z_buffer(SGE *engine)
	{
		for (size_t i = 0; i < engine->console->size; i++)
			engine->z_buffer[i] = 1000;
	}

	// =============================================================================
	// @@@ + SGE_check_screen_pos
	// =============================================================================
	bool SGE_check_screen_pos(SGE *engine, const COORD *screen_pos)
	{
		return true
			&& screen_pos->X >= 0
			&& screen_pos->X < engine->console->width
			&& screen_pos->Y >= 0
			&& screen_pos->Y < engine->console->height;
	}

#endif
#endif
