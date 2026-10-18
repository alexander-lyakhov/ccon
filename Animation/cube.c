#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <conio.h>
#include <math.h>
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

typedef struct App {
	Console *console;
	Camera *camera;
	Plane *plane;
	Cube *cube;

	float *z_buffer;

	float ks; // general screen scale
	float kx; // screen x scale (actual for console)

	uint8_t debug;
} App;

void app_render_buff(Console *console)
{
	WriteConsoleOutputCharacter(
		console->handle,
		console->buff,
		console->size,
		(COORD){0, 0},
		&console->written
	);
}

void app_render_attrs(Console *console)
{
	WriteConsoleOutputAttribute(
		console->handle,
		console->attrs,
		console->size,
		(COORD){0, 0},
		&console->written
	);
}

// =============================================================================
// @@@ + App_render
// =============================================================================
void App_render(Console *console)
{
	app_render_buff(console);
	app_render_attrs(console);
}

// =============================================================================
// @@@ + project2D
// =============================================================================
Vec2D project2D(App *app, const Vec3D p)
{
	if (p.z <= 0)
		return (Vec2D){ .x = 1000, .y = 1000 }; // Return values that are too big so that the point isnt't drawn on the screen

	return (Vec2D) {
		.x = p.x / p.z,
		.y = p.y / p.z,
	};
}

// =============================================================================
// @@@ + pointToScreen
// =============================================================================
COORD pointToScreen(App *app, Vec2D p)
{
	return (COORD) {
		round(app->console->width  * (1 + p.x * app->kx) / 2),
		round(app->console->height * (1 - p.y * app->ks) / 2),
	};
}

// =============================================================================
// @@@ + App_show_debug_info
// =============================================================================
void App_show_debug_info(App *app)
{
	sprintf(app->console->buff + app->console->width *  0, "Camera X rotation: %7.3f", app->camera->origin.rx);
	sprintf(app->console->buff + app->console->width *  1, "Camera Y rotation: %7.3f", app->camera->origin.ry);
	sprintf(app->console->buff + app->console->width *  2, "Camera Z rotation: %7.3f", app->camera->origin.rz);
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
		for (int col = 0; col < 26; col++)
			(app->console->attrs + app->console->width * row)[col] = 0x07;
}

// =============================================================================
// @@@ + App_update
// =============================================================================
void App_update(App *app)
{
	Console_mem_fill(app->console, " ", 0x03);
	// memset(app->console->buff, ' ', app->console->size);

	for (size_t i = 0; i < app->console->size; i++)
		app->z_buffer[i] = 1000;

	if (app->cube->is_rotating)
	{
		Cube_rotate_x(app->cube, 0.025);
		Cube_rotate_y(app->cube, 0.05);
		Cube_rotate_z(app->cube, 0);
	}

	size_t buff_size = SIZE * SIZE;

	for (size_t face_index = 0; face_index < app->cube->size; face_index++)
	{
		Plane *face = Cube_set_current_face(app->cube, face_index);

		for (size_t i = 0; i < buff_size; i++)
		{
			Vec3D p = face->buff[i];

			p =   Cube_calculate_single_point(app->cube, &p);
			p = Camera_calculate_single_point(app->camera, &p);

			COORD screen_pos = pointToScreen(
				app, project2D(app, p)
			);

			if (false
				|| screen_pos.X < 0
				|| screen_pos.X >= app->console->width
				|| screen_pos.Y < 0
				|| screen_pos.Y >= app->console->height
			) continue;

			int index = screen_pos.Y * app->console->width + screen_pos.X;

			if ((index < app->console->size) && (p.z < app->z_buffer[index]))
			{
				((char*)app->console->buff)[index] = face->texture;
				app->z_buffer[index] = p.z;
			}
		}
	}

	if (app->debug)
		App_show_debug_info(app);
}

// =============================================================================
// @@@ + App_listen
// =============================================================================
uint8_t App_listen(App *app)
{
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
				app->debug ^= 1;
				break;
		}
	}

	return 1;
}

// =============================================================================
// @@@ + App_free_mem
// =============================================================================
void App_free_mem(App *app)
{
	Console_mem_free(app->console);
	Plane_free_static_buff(&(app->cube->face[0]));
	free(app->z_buffer);
}

int main()
{
	system("cls");

	Console console = Console_create();
	// Console_mem_fill(&console, " ", 0x03);

	float font_ar = console.font_ar;
	if (!font_ar) font_ar = 0.462; // fallback for my Windows Terminal

	float screen_ar = (float)console.width / console.height;
	float ks = 2.5;
	float kx = ks / (screen_ar * font_ar);

	Camera camera = Camera_create(
		(Vec3D){0, 0, -4}
	);
	
	Cube cube = Cube_create(
		(Vec3D){0, 0, 4}
	);

	App app = {
		.console  = &console,
		.camera   = &camera,
		.cube     = &cube,
		.z_buffer = malloc(sizeof(float) * console.size),
		.ks       = ks,
		.kx       = kx,
		.debug    = 0,
	};

	CURSOR_INIT(&app);
	CURSOR_HIDE(&app);
	
	while (App_listen(&app))
	{
		App_render(&console);
		App_update(&app);
		usleep(5000);
	}

	App_free_mem(&app);

	CURSOR_SHOW(&app);

	return 0;
}