#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <unistd.h>

#include "h/macros.h"

#define CONSOLE_IMPLEMENTATION
#include "h/console.h"

#define VEC3D_IMPLEMENTATION
#include "h/vec.h"

#define CAMERA_IMPLEMENTATION
#include "h/camera.h"

#define PLANE_IMPLEMENTATION
#include "h/plane.h"

#define CUBE_IMPLEMENTATION
#include "h/cube.h"

#define SGE_IMPLEMENTATION
#include "h/engine.h"

typedef enum {
	CUBE_COLOR = 0x07,
	INFO_COLOR = 0x07,

} Color;

typedef struct App {
	Console *console;
	Camera *camera;
	Plane *plane;
	Cube *cube;
	SGE *engine;

	uint8_t is_debug_mode;

} App;

// =============================================================================
// @@@ + App_free
// =============================================================================
void App_free(App *app)
{
	Console_mem_free(app->console);
	   Cube_mem_free(app->cube);
	    SGE_mem_free(app->engine);
}

// =============================================================================
// @@@ + App_show_debug_info
// =============================================================================
void App_show_debug_info(App *app)
{
	sprintf(app->console->buff + app->console->width *  0, "%-17s: %7.3f", "Camera X rotation", app->camera->origin.rx);
	sprintf(app->console->buff + app->console->width *  1, "%-17s: %7.3f", "Camera Y rotation", app->camera->origin.ry);
	sprintf(app->console->buff + app->console->width *  2, "%-17s: %7.3f", "Camera Z rotation", app->camera->origin.rz);

	sprintf(app->console->buff + app->console->width *  3, "--------------------------");
	
	sprintf(app->console->buff + app->console->width *  4,  "%-17s: %7.3f", "Camera X" ,app->camera->origin.x);
	sprintf(app->console->buff + app->console->width *  5,  "%-17s: %7.3f", "Camera Y", app->camera->origin.y);
	sprintf(app->console->buff + app->console->width *  6,  "%-17s: %7.3f", "Camera Z", app->camera->origin.z);

	sprintf(app->console->buff + app->console->width *  7, "--------------------------");

	sprintf(app->console->buff + app->console->width *  8,  "%-17s: %7.3f", "Cube X rotation", app->cube->origin.rx);
	sprintf(app->console->buff + app->console->width *  9,  "%-17s: %7.3f", "Cube Y rotation", app->cube->origin.ry);
	sprintf(app->console->buff + app->console->width * 10,  "%-17s: %7.3f", "Cube Z rotation", app->cube->origin.rz);

	sprintf(app->console->buff + app->console->width * 11, "--------------------------");

	sprintf(app->console->buff + app->console->width * 12, "%-17s: %7.3f", "Cube X", app->cube->origin.x);
	sprintf(app->console->buff + app->console->width * 13, "%-17s: %7.3f", "Cube Y", app->cube->origin.y);
	sprintf(app->console->buff + app->console->width * 14, "%-17s: %7.3f", "Cube Z", app->cube->origin.z);

	for (int row = 0; row <= 14; row++)
	{
		for (int col = 0; col < 26; col++)
			(app->console->attrs + app->console->width * row)[col] = INFO_COLOR;

		char *buff_line = app->console->buff + app->console->width * row;

		if (buff_line[26] == 0)
			buff_line[26] = ' ';
	}
}

// =============================================================================
// @@@ + App_update
// =============================================================================
void App_update(App *app)
{
	Console_mem_fill(app->console, " ", CUBE_COLOR);
	// memset(app->console->buff, ' ', app->console->size);
	SGE_reset_z_buffer(app->engine);

	if (app->cube->is_rotating)
	{
		Cube_rotate_x(app->cube, 0.025);
		Cube_rotate_y(app->cube, 0.025);
		Cube_rotate_z(app->cube, 0.025);
	}

	for (size_t face_index = 0; face_index < app->cube->size; face_index++)
	{
		Plane *face = Cube_set_current_face(app->cube, face_index);
		
		for (size_t i = 0; i < face->buff_size; i++)
		{
			//
			// Get point from the plane
			//
			Vec3D p = face->buff[i];

			//
			// Rotate this point first related to the cube, then related to the camera view
			//
			p =   Cube_calculate_single_point(app->cube, &p);
			p = Camera_calculate_single_point(app->camera, &p);

			//
			// Project this point into the screen
			//
			COORD screen_pos = SGE_point_to_screen(
				app->engine, SGE_project_2d(p)
			);

			//
			// Check if the point is inside the screen
			//
			if (!SGE_check_screen_pos(app->engine, &screen_pos))
				continue;

			//
			// Get target console index to draw the ascii_char
			//
			int index = screen_pos.Y * app->console->width + screen_pos.X;

			//
			// Check if target index is inside the screen and the point is the closest to the camera
			//
			if (!(index < app->console->size && p.z < app->engine->z_buffer[index]))
				continue;

			app->engine->z_buffer[index] = p.z;

			//
			// Draw ascii_char to console target position
			//
			((char*)app->console->buff)[index] = face->ascii_char;

			//
			// Check if the cube texture needs to be displayed
			//
			if (Cube_should_display_texture(app->cube))
			{
				((char*)app->console->buff)[index] = app->cube->texture[i] == '#'
					? face_index < 3 ? '#' : '.'
					: face->ascii_char;
			}
		}
	}

	if (app->is_debug_mode)
		App_show_debug_info(app);
}

// =============================================================================
// @@@ + App_resize
// =============================================================================
uint8_t App_resize(App *app)
{
	Console_mem_reset(app->console);
	SGE_reset(app->engine);

	return 1;
}

// =============================================================================
// @@@ + App_listen
// =============================================================================
uint8_t App_listen(App *app)
{
	if (Console_check_resize(app->console))
		return App_resize(app);

	if (_kbhit())
	{
		char key = _getch();

		if (key == 27 || ((key | 32) == 'q')) return 0;

		// Rotate camera keys (cursor keys)
		//------------------------------------------
		switch(key) {
			case KEY_LEFT:
				Camera_rotate_y(app->camera, -0.05);
				return 1;

			case KEY_RIGHT:
				Camera_rotate_y(app->camera, 0.05);
				return 1;

			case KEY_UP:
				Camera_rotate_x(app->camera, 0.05);
				return 1;

			case KEY_DOWN:
				Camera_rotate_x(app->camera, -0.05);
				return 1;
		}

		switch(key | 32) {
			
			// Move camera keys (a, s, w, d)
			//----------------------------------------
			case 's':
				Camera_translate_z(app->camera, -0.1);
				break;

			case 'w':
				Camera_translate_z(app->camera, 0.1);
				break;

			case 'a':
				Camera_translate_x(app->camera, 0.1);
				break;

			case 'd':
				Camera_translate_x(app->camera, -0.1);
				break;

			// Move cube keys ( j, k, h, l )
			//----------------------------------------
			case 'j':
				Cube_translate_x(app->cube, -0.1);
				break;

			case 'k':
				Cube_translate_x(app->cube, 0.1);
				break;

			case 'h':
				Cube_translate_y(app->cube, -0.1);
				break;

			case 'l':
				Cube_translate_y(app->cube, 0.1);
				break;
			
			// Other
			//----------------------------------------
			case ' ':
				Cube_toggle_rotate(app->cube);
				break;

			case 'r':
				Camera_reset(app->camera);
				Cube_reset(app->cube);
				break;

			case 'i':
				app->is_debug_mode ^= 1;
				break;

			case 't':
				app->cube->is_texture_visible ^= 1;
				break;
		}
	}

	return 1;
}

// =============================================================================
// @@@ + App_render
// =============================================================================
void App_render(Console *console)
{
	Console_render(console);
}

int main()
{
	system("cls");

	Console console = Console_create();
	Console_mem_fill(&console, " ", CUBE_COLOR);

	Camera camera = Camera_create(
		(Vec3D){0, 0, -3}
	);
	
	Cube cube = Cube_create(
		(Vec3D){0, 0, 4}
	);

	Cube_load_texture(
		&cube, "texture.txt"
	);

	SGE engine = {
		.console  = &console,
		.z_buffer = NULL,
		.scale    = 2.5,
	};
	SGE_init(&engine);

	App app = {
		.console       = &console,
		.camera        = &camera,
		.cube          = &cube,
		.engine        = &engine,
		.is_debug_mode = 0,
	};
	
	CURSOR_INIT(&app);
	CURSOR_HIDE(&app);
	
	while (App_listen(&app))
	{
		App_render(&console);
		App_update(&app);
		usleep(5000);
	}
	
	App_free(&app);

	CURSOR_SHOW(&app);

	return 0;
}
