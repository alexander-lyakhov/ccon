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
	uint8_t is_smoke_enabled;
} App;

typedef void (*CallbackFn)(App *app, size_t index);

// =============================================================================
// @@@ + App_free
// =============================================================================
void App_free(App *app)
{
	Console_mem_free(app->console);
}

// =============================================================================
// @@@ + App_first_frame
// =============================================================================
void App_first_frame(App *app, size_t index)
{
	Console *console = app->console;

	WORD curr_line_attr = console->attrs[index];
	WORD prev_line_attr = console->attrs[index + console->width];
		
	WORD attr = RAND(100) > 50 && prev_line_attr > 0
		? (prev_line_attr - 1) & 0x0e
		: prev_line_attr & 0x0e;
	
	// if (attr <= 0x0d) attr &= 0x0d;
	if (attr < 0x0c) attr &= 4;

	console->attrs[index] = attr;
}

// =============================================================================
// @@@ + App_next_frame
// =============================================================================
void App_next_frame(App *app, size_t index)
{
	Console *console = app->console;

	WORD curr_line_attr = console->attrs[index];
	WORD prev_line_attr = console->attrs[index + console->width];

	int value = RAND(100);

	if (value > 70 && prev_line_attr)
	{
		WORD attr = prev_line_attr;
		
		char offset = RAND(3) - 1;

		if (RAND(100) > 75)
			attr &= 0x0e;

		if (attr < 0x0c)
			attr &= 4;

		console->attrs[index + offset] = attr;

		return;
	}

	if (value > 62 && curr_line_attr)
	{
		WORD attr = (curr_line_attr - 1) & 0x0e;
		
		if (attr <= 0x0d) attr &= 0x0d;
		// if (attr < 0x0c) attr &= 4;
		
		console->attrs[index] = attr;
	}
}

// =============================================================================
// @@@ + App_update
// =============================================================================
void App_update(App *app, CallbackFn callback)
{
	for (int row = app->console->height - 2; row >= 0; row--)
		for (int col = 0; col < app->console->width; col++)
			callback(app, row * app->console->width + col);
}

// =============================================================================
// @@@ + App_listen
// =============================================================================
uint8_t App_listen(App *app)
{
	if (_kbhit())
	// if (1)
	{
		char key = _getch();

		if (key == 27 || ((key | 32) == 'q')) return 0;

		if ((key | 32) == 's')
			app->is_smoke_enabled ^= 1;
	}
	return 1;
}

int main()
{
	system("cls");

	INIT_RAND;

	Console console = Console_create();
	Console_mem_fill(&console, "@", 0x0000);

	for (int i = console.size - console.width; i < console.size; i++)
		console.attrs[i] = 0x0f;

	App app = {
		.console = &console,
		.is_smoke_enabled = 1,
	};

	App_update(&app, App_first_frame);
	
	Console_render_buff(&console);

	while (App_listen(&app))
	{
		App_update(&app, App_next_frame);
		Console_render_attrs(&console);
		usleep(20000);
		// Sleep(40);
	}

	App_free(&app);

	return 0;
}