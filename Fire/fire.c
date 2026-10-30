#include <stdio.h>
#include <stdint.h>
#include <conio.h>
#include <time.h>
#include <windows.h>
#include <unistd.h>

#define INIT_RAND srand(time(NULL))
#define RAND(range) (rand() % range)

#define CONSOLE_IMPLEMENTATION
#include "h/console.h"

typedef struct {
	Console *console;
	uint8_t render_ones;
} App;

// =============================================================================
// @@@ + App_free
// =============================================================================
void App_free(App *app)
{
	Console_mem_free(app->console);
}

// =============================================================================
// @@@ + App_first_pass
// =============================================================================
void App_first_pass(App *app)
{
	Console *console = app->console;

	for (int i = 0; i < console->width; i ++)
		console->attrs[i] = 0x0f;

	for (int row = 1; row < console->height; row++)
	{
		for (int col = 0; col < console->width; col++)
		{
			size_t index = row * console->width + col;
			
			WORD curr_line_attr = console->attrs[index];
			WORD prev_line_attr = console->attrs[index - console->width];

			console->attrs[index] = RAND(100) > 50 && prev_line_attr > 0
				? prev_line_attr - 1
				: prev_line_attr;
		}
	}
}
// =============================================================================
// @@@ + App_update
// =============================================================================
void App_update(App *app)
{
	Console *console = app->console;

	for (int row = 1; row < console->height; row++)
	{
		for (int col = 0; col < console->width; col++)
		{
			size_t index = row * console->width + col;
			
			WORD curr_line_attr = console->attrs[index];
			WORD prev_line_attr = console->attrs[index - console->width];

			console->attrs[index] = RAND(100) > 50 && prev_line_attr
				? curr_line_attr ? prev_line_attr - 1 : 0x0000
				: curr_line_attr ? curr_line_attr - 1 : 0x0000;
		}
	}
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
	}
	return 1;
}

int main()
{
	system("cls");

	INIT_RAND;

	Console console = Console_create();
	Console_mem_fill(&console, "@", 0x0000);

	App app = {
		.console     = &console,
		.render_ones = 0,
	};

	App_first_pass(&app);

	while (App_listen(&app))
	{
		// App_render(&console);
		
		Console_render(&console);
		App_update(&app);
		// usleep(100000);
		Sleep(50);
	}

	App_free(&app);


	// _getch();

	return 0;
}