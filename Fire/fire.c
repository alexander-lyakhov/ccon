#include <stdio.h>
#include <stdint.h>
#include <conio.h>
#include <time.h>
#include <windows.h>
#include <unistd.h>

#define INIT_RAND srand(time(NULL))
#define RAND(range) (rand() % range)
#define BITS_OFFSET 4

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
// @@@ + App_init_frame
// =============================================================================
void App_init_frame(App *app)
{
	for (int i = app->console->size - app->console->width; i < app->console->size; i++)
		app->console->attrs[i] = 0xf0;
}

// =============================================================================
// @@@ + App_first_frame
// =============================================================================
void App_first_frame(App *app, size_t index)
{
	Console *console = app->console;

	WORD curr_line_attr = console->attrs[index] >> BITS_OFFSET;
	WORD prev_line_attr = console->attrs[index + console->width] >> BITS_OFFSET;
		
	WORD attr = RAND(100) > 50 && prev_line_attr > 0
		? (prev_line_attr - 1) & 0x0e
		: prev_line_attr & 0x0e;
	
	// if (attr <= 0x0d) attr &= 0x0d;
	if (attr < 0x0c) attr &= 4;

	console->attrs[index] = attr << BITS_OFFSET;
}

// =============================================================================
// @@@ + App_next_frame
// =============================================================================
void App_next_frame(App *app, size_t index)
{
	Console *console = app->console;

	WORD curr_line_attr = console->attrs[index] >> BITS_OFFSET;
	WORD prev_line_attr = console->attrs[index + console->width] >> BITS_OFFSET;

	int value = RAND(100);

	if (value > 70 && prev_line_attr)
	{
		WORD attr = prev_line_attr;
		
		char offset = index ? RAND(3) - 1 : 0;

		if (RAND(100) > 80)
			attr &= 0x0e;

		if (attr < 0x0c)
			attr &= 0x04;

		console->attrs[index + offset] = attr << BITS_OFFSET;

		return;
	}

	if (value > 62 && curr_line_attr)
	{
		WORD attr = (curr_line_attr - 1) & 0x0e;
		
		if (app->is_smoke_enabled) {
			if (attr <= 0x0d) attr &= 0x0d;
		}
		else {
			if (attr < 0x0c) attr &= 0x04;
		}
		console->attrs[index] = attr << BITS_OFFSET;
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
// @@@ + App_resize
// =============================================================================
uint8_t App_resize(App *app)
{
	Console_mem_reset(app->console);
	
	App_init_frame(app);
	App_update(app, App_first_frame);

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
	// if (1)
	{
		char key = _getch();

		if (key == 27 || ((key | 32) == 'q'))
			return 0;

		if ((key | 32) == 's')
			app->is_smoke_enabled ^= 1;
	}
	return 1;
}

int main()
{
	system("cls");
	system("colortool/colortool.exe -q schemes/cmd-legacy.ini");

	INIT_RAND;

	Console console = Console_createW();
	Console_mem_fill(&console, L"░", 0x0000);

	App app = {
		.console = &console,
		.is_smoke_enabled = 0,
	};

	App_init_frame(&app);
	App_update(&app, App_first_frame);
	
	Console_render_buffW(&console);

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