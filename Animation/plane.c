#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <math.h>
#include <windows.h>
#include <unistd.h>

#define CONSOLE_IMPLEMENTATION
#include "h/console.h"
#include "h/macros.h"

#define SIZE 80

typedef struct Transform {
	float x;
	float y;
	float z;

	float tx;
	float ty;
	float tz;

	float rx;
	float ry;
	float rz;

} Transform;

typedef struct Vec2D {
	float x;
	float y;
} Vec2D;

typedef struct Vec3D {
	float x;
	float y;
	float z;
} Vec3D;

typedef struct Plane {
	Transform origin;
	Vec3D *buff;
} Plane;

typedef struct App {
	Console *console;
	Plane *plane;
	// Vec3D *buff;

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
// @@@ + translateZ
// =============================================================================
uint8_t translateZ(App *app, float dz)
{
	size_t buff_size = SIZE * SIZE;

	for (size_t i = 0; i < buff_size; i++)
	{
		app->plane->buff[i].z += dz;

		if (app->plane->buff[i].z < -1)
			app->plane->buff[i].z = -1;
	}

	return 1;
}

// =============================================================================
// @@@ + pointToScreen
// =============================================================================
COORD pointToScreen(App *app, Vec2D *p)
{
	return (COORD) {
		round(app->console->width  * (1 + p->x * app->kx)) / 2,
		round(app->console->height * (1 + p->y * app->ks)) / 2,
	};
}

// =============================================================================
// @@@ + project2D
// =============================================================================
Vec2D project2D(Vec3D *p)
{
	return (Vec2D) {
		.x = p->x / p->z,
		.y = p->y / p->z,
	};
}

// =============================================================================
// @@@ + rotate_y
// =============================================================================
Vec3D rotate_y(Vec3D *p)
{
	return (Vec3D) {
		p->x, p->y, p->z
	};
}

// =============================================================================
// @@@ + app_update
// =============================================================================
void app_update(App *app)
{
	Console_mem_fill(app->console, " ", 0x03);
	
	app->plane->origin.ry += 0.05;

	size_t buff_size = SIZE * SIZE;

	for (size_t i = 0; i < buff_size; i++)
	{
	    Vec3D p = rotate_y(&app->plane->buff[i]);
		Vec2D point = project2D(&p);
		COORD screen_pos = pointToScreen(app, &point);

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
			translateZ(app, 0.05);
		}

		if ((key | 32) == 's') {
			translateZ(app, -0.05);
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

	plane.origin.z = -1;

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
				.z = 1,
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