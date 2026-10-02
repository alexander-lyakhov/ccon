#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <math.h>
#include <windows.h>
#include <unistd.h>

#define CONSOLE_IMPLEMENTATION
#include "h/console.h"
#include "h/macros.h"

#define PLANE_IMPLEMENTATION
#include "h/plane.h"

#define SIZE 80

typedef struct App {
	Console *console;
	Plane *plane;

	float ks;
	float kx;
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
// @@@ + pointToScreen
// =============================================================================
COORD pointToScreen(App *app, Vec2D p)
{
	return (COORD) {
		round(app->console->width  * (1 + p.x * app->kx)) / 2,
		round(app->console->height * (1 + p.y * app->ks)) / 2,
	};
}

// =============================================================================
// @@@ + project2D
// =============================================================================
Vec2D project2D(Transform *origin, Vec3D p)
{
	return (Vec2D) {
		.x = p.x / (p.z + origin->z),
		.y = p.y / (p.z + origin->z),
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
				&app->plane->origin,
				// rotate_z(app->plane, rotate_y(app->plane, rotate_x(app->plane, p)))
				rotate_y(app->plane, p)
			)
		);

		if (screen_pos.X < 0 || screen_pos.X >= app->console->width || screen_pos.Y < 0 || screen_pos.Y >= app->console->height)
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

		if ((key | 32) == 'w') {
			plane_translate_z(app->plane, -0.1);
		}

		if ((key | 32) == 's') {
			plane_translate_z(app->plane, 0.1);
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
	float screen_ar = (float)console.width / console.height;
	float ks = 0.75;                        // general screen scale
	float kx = ks / (screen_ar * font_ar);  // screen x scale (actual for console)

	float kx_distribution = (float)SIZE / (SIZE - 1);
	float ky_distribution = (float)SIZE / (SIZE - 1);

	Plane plane = {
		.buff = malloc(SIZE * SIZE * sizeof(Vec3D))
	};

	plane.origin.z = 3;

	App app = {
		.console = &console,
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
	/*
	size_t buff_size = SIZE * SIZE;

	for (size_t i = 0; i < buff_size; i++)
	{
		Vec2D point = project2D(&buff[i]);
		COORD screen_pos = pointToScreen(&app, &point);

		int index = screen_pos.Y * console.width + screen_pos.X;

		if (index < console.size)
			((char*)console.buff)[index] = '$';
	}
	*/

	// app_render(&console);

	// printf("%f", kx);
	
	while (app_listen(&app))
	{
		app_render(&console);
		app_update(&app);
		usleep(10000);
	}
	
	Console_mem_free(&console);
	free(plane.buff);

	CURSOR_SHOW(&app);

	return 0;
}