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

typedef struct Vec2D {
	float x;
	float y;
} Vec2D;

typedef struct App {
	Console *console;
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
/*void app_update(App *app)
{
}
*/

// =============================================================================
// @@@ + pointToScreen
// =============================================================================
COORD pointToScreen(App *app, Vec2D *pos)
{
	return (COORD) {
		round(app->console->width  * (1 + pos->x * app->kx)) / 2,
		round(app->console->height * (1 + pos->y * app->ks)) / 2,
	};
}

int main()
{
	system("cls");

	Console console = Console_create();
	Console_mem_fill(&console, " ", 0x03);

	float font_ar = console.font_ar;
	float screen_ar = (float)console.width / console.height;
	float ks = 0.75;                        // general screen scalse
	float kx = ks / (screen_ar * font_ar); // screen x scale

	float kx_distribution = (float)SIZE / (SIZE - 1);
	float ky_distribution = (float)SIZE / (SIZE - 1);

	Vec2D *buff = malloc(SIZE * SIZE * sizeof(Vec2D));

	App app = {
		.console = &console,
		.ks = ks,
		.kx = kx,
	};

	CURSOR_INFO(&app);
	CURSOR_HIDE(&app);

	for (size_t row = 0, index = 0; row < SIZE; row++)
	{
		for (size_t col = 0; col < SIZE; col++)
		{
			buff[index++] = (Vec2D) {
				.x = (float)col / SIZE * kx_distribution * 2 - 1,
				.y = (float)row / SIZE * ky_distribution * 2 - 1,
			};
		}
	}

	size_t buff_size = SIZE * SIZE;

	for (size_t i = 0; i < buff_size; i++)
	{
		COORD screen_pos = pointToScreen(&app, &buff[i]);

		int index = screen_pos.Y * console.width + screen_pos.X;

		if (index < console.size)
			((char*)console.buff)[index] = '$';
	}

	app_render(&console);

	// printf("%f", kx);

	/*
	while (app_listen(&console))
	{
		app_render(&console);
		app_update(&app);
		usleep(20000);
	}
	*/
	
	Console_mem_free(&console);
	free(buff);

	CURSOR_SHOW(&app);

	return 0;
}