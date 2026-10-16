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

// #define SIZE 80

typedef struct App {
	Console *console;
	Camera *camera;
	Plane *plane;
	Cube *cube;

	float *z_buffer;

	float ks; // general screen scale
	float kx; // screen x scale (actual for console)
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
// @@@ + app_render
// =============================================================================
void app_render(Console *console)
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
// @@@ + app_update
// =============================================================================
void app_update(App *app)
{
	// Console_mem_fill(app->console, " ", 0x03);
	memset(app->console->buff, ' ', app->console->size);

	for (size_t i = 0; i < app->console->size; i++)
		app->z_buffer[i] = 1000;

	// Cube_rotate_x(app->cube, 0.025);
	// Cube_rotate_y(app->cube, 0.05);
	// Cube_rotate_z(app->cube, 0);

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
	sprintf(app->console->buff + app->console->width * 0, "camera->origin.rx: %7.3f", app->camera->origin.rx);
	sprintf(app->console->buff + app->console->width * 1, "camera->origin.ry: %7.3f", app->camera->origin.ry);
	sprintf(app->console->buff + app->console->width * 2, "camera->origin.rz: %7.3f", app->camera->origin.rz);

	sprintf(app->console->buff + app->console->width * 3, "camera->origin.x: %8.3f", app->camera->origin.x);
	sprintf(app->console->buff + app->console->width * 4, "camera->origin.y: %8.3f", app->camera->origin.y);
	sprintf(app->console->buff + app->console->width * 5, "camera->origin.z: %8.3f", app->camera->origin.z);

	sprintf(app->console->buff + app->console->width * 6, "  cube->origin.x: %8.3f", app->cube->origin.x);
	sprintf(app->console->buff + app->console->width * 7, "  cube->origin.y: %8.3f", app->cube->origin.y);
	sprintf(app->console->buff + app->console->width * 8, "  cube->origin.z: %8.3f", app->cube->origin.z);
}

// =============================================================================
// @@@ + app_listen
// =============================================================================
uint8_t app_listen(App *app)
{
	if (_kbhit())
	{
		char key = _getch();

		if (key == 27 || ((key | 32) == 'q')) return 0;

		switch(key | 32) {

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
		}

		// Rotate camera keys (cursor keys)
		//------------------------------------------
		switch(key) {
			case KEY_LEFT:
				Camera_rotate_y(app->camera, -0.05);
				break;

			case KEY_RIGHT:
				Camera_rotate_y(app->camera, 0.05);
				break;

			case KEY_UP:
				Camera_rotate_x(app->camera, 0.05);
				break;

			case KEY_DOWN:
				Camera_rotate_x(app->camera, -0.05);
				break;
		}
	}

	return 1;
}

int main()
{
	system("cls");

	Console console = Console_create();
	Console_mem_fill(&console, " ", 0x03);

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
	};

	CURSOR_INFO(&app);
	CURSOR_HIDE(&app);

	// app_render(&console);

	// printf("%f", kx);
	
	while (app_listen(&app))
	{
		app_render(&console);
		app_update(&app);
		usleep(5000);
	}
	
	Console_mem_free(&console);
	// free(plane.buff);
	Plane_free(&cube.face[0]);
	free(app.z_buffer);

	CURSOR_SHOW(&app);

	return 0;
}