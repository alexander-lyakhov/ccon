#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <math.h>
#include <windows.h>
#include <unistd.h>

#define CONSOLE_IMPLEMENTATION
#include "h/console.h"
#include "h/macros.h"

#define CAMERA_IMPLEMENTATION
#include "h/camera.h"

#define PLANE_IMPLEMENTATION
#include "h/plane.h"

#define SIZE 80

typedef struct App {
	Console *console;
	Camera *camera;
	Plane *plane;

	float ks; // general screen scale
	float kx; // screen x scale (actual for console)
	float angle;
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
Vec2D project2D(App *app, Vec3D p)
{
	Origin *plane_origin  = &app->plane->origin;
	Origin *camera_origin = &app->camera->origin;

	p.x +=  camera_origin->x + plane_origin->x;
	p.y +=  camera_origin->y + plane_origin->y;
	p.z += -camera_origin->z + plane_origin->z;

	Vec3D p1 = Camera_rotate_y(app->camera, p);

	if (p1.z <= 0)
    	return (Vec2D){ .x = 1000, .y = 1000 }; // Return values that are too big so that the point isnt't drawn on the screen

	return (Vec2D) {
		.x = p1.x / p1.z,
		.y = p1.y / p1.z,
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
	Console_mem_fill(app->console, " ", 0x03);
	
	app->plane->origin.rx += 0.05;
	app->plane->origin.ry += 0.05;
	app->plane->origin.rz += 0.05;

	size_t buff_size = SIZE * SIZE;

	for (size_t i = 0; i < buff_size; i++)
	{
		Vec3D p = app->plane->buff[i];

		COORD screen_pos = pointToScreen(app,
			project2D(
				app,
				// plane_rotate_z(app->plane, plane_rotate_y(app->plane, plane_rotate_x(app->plane, p)))
				Plane_rotate_y(app->plane, p)
			)
		);

		if (screen_pos.X < 0 ||
			screen_pos.X >= app->console->width ||
			screen_pos.Y < 0 ||
			screen_pos.Y >= app->console->height
		)
			continue;

		int index = screen_pos.Y * app->console->width + screen_pos.X;

		if (index < app->console->size)
			((char*)app->console->buff)[index] = '$';
	}
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

		if ((key | 32) == '-') {
			Camera_translate_z(app->camera, -0.1);
		}

		if ((key | 32) == '+') {
			Camera_translate_z(app->camera, 0.1);
		}

		if ((key | 32) == 'a') {
			Plane_translate_x(app->plane, -0.1);
			// Camera_translate_x(app->camera, -0.1);
		}

		if ((key | 32) == 'd') {
			Plane_translate_x(app->plane, 0.1);
			// Camera_translate_x(app->camera, 0.1);
		}

		if ((key | 32) == 'w') {
			Plane_translate_y(app->plane, 0.1);
		}

		if ((key | 32) == 's') {
			Plane_translate_y(app->plane, -0.1);
		}

		if ((key | 32) == 'o') {
			app->camera->origin.ry -= 0.05;
		}

		if ((key | 32) == 'p') {
			app->camera->origin.ry += 0.05;
		}
	}

	return 1;
}

int main()
{
	system("cls");

	Console console = Console_create();
	Console_mem_fill(&console, " ", 0x03);

	Camera camera = Camera_create((Vec3D){0, 0, -4});

	float font_ar = console.font_ar;
	float screen_ar = (float)console.width / console.height;
	float ks = 1;
	float kx = ks / (screen_ar * font_ar);

	float kx_distribution = (float)SIZE / (SIZE - 1);
	float ky_distribution = (float)SIZE / (SIZE - 1);

	Plane plane = {
		.buff = malloc(SIZE * SIZE * sizeof(Vec3D))
	};

	App app = {
		.console = &console,
		.camera  = &camera,
		.plane   = &plane,
		.ks      = ks,
		.kx      = kx,
	};

	CURSOR_INFO(&app);
	CURSOR_HIDE(&app);

	for (size_t row = 0, index = 0; row < SIZE; row++)
	{
		for (size_t col = 0; col < SIZE; col++)
		{
			plane.buff[index++] = (Vec3D) {
				.x = (float)col / SIZE * kx_distribution * 2 - 1,
				.y = (float)row / SIZE * ky_distribution * 2 - 1,
				.z = 0,
			};
		}
	}

	// app_render(&console);

	// printf("%f", kx);
	
	while (app_listen(&app))
	{
		app_render(&console);
		app_update(&app);
		usleep(5000);
	}
	
	Console_mem_free(&console);
	free(plane.buff);

	CURSOR_SHOW(&app);

	return 0;
}