#include <stdio.h>
#include <conio.h>
#include <math.h>
#include <windows.h>
#include <unistd.h>

#define CONSOLE_IMPLEMENTATION
#include "h/console.h"
#include "h/macros.h"

typedef struct App {
	Console *console;
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
// @@@ + app_listen
// =============================================================================
uint8_t app_listen(Console *console)
{
	if (_kbhit())
	{
		char key = _getch();
		if (key == 27 || ((key | 32) == 'q')) return 0;
	}

	return 1;
}

// =============================================================================
// @@@ + app_update
// =============================================================================
void app_update(App *app)
{
	Console *console = app->console;

	app->angle += 0.05;

	size_t index = 0;

	for (int row = 0; row < console->height; row++)
	{
		for (int col = 0; col < console->width; col++)
		{
			float x = (col * 2.0 / console->width - 1) * app->kx + sin(app->angle);
			float y = (console->height - row) * 2.0 / console->height - 1;

			float r = sqrt(x * x + y * y);

			((char*)console->buff)[index++] = r > .5 ? ' ' : '$';
		}
	}
}

int main()
{
	system("cls");

	Console console = Console_create();
	Console_mem_fill(&console, " ", 0x03);

	int cx = console.width  >> 1;
	int cy = console.height >> 1;

	float font_ar = console.font_ar;
	float screen_ar = (float)console.width / console.height;
	float kx = screen_ar * font_ar;

	int vp_size = fmin(floor(console.width * console.font_ar), console.height);
	/*
	printf("w = %d, h = %d, vp_size = %d, font_ar = %f",
		console.width,
		console.height,
		vp_size,
		console.font_ar
	);
	*/
	App app = {
		.console = &console,
		.kx = kx,
		.angle = 0.0,
	};

	CURSOR_INFO(&app);
	CURSOR_HIDE(&app);

	// _getch();

	/*
	size_t index = 0;
	
	for (int row = 0; row < console.height; row++)
	{
		for (int col = 0; col < console.width; col++)
		{
			float x = (col - cx) * font_ar;
			float y = (row - cy);

			int r = sqrt(x * x + y * y);

			((char*)console.buff)[index++] = r >= (vp_size / 2) ? ' ' : '$';
		}
	}
	*/

	while (app_listen(&console))
	{
		app_render(&console);
		app_update(&app);
		usleep(20000);
	}
	
	Console_mem_free(&console);

	// _getch();

	CURSOR_SHOW(&app);

	return 0;
}