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

	for (int i = console->size - console->width; i < console->size; i++)
		console->attrs[i] = 0x0f;

	for (int row = console->height - 2; row >= 0; row--)
	{
		for (int col = 0; col < console->width; col++)
		{
			size_t index = row * console->width + col;
			
			WORD curr_line_attr = console->attrs[index];
			WORD prev_line_attr = console->attrs[index + console->width];
			
			/*console->attrs[index] = RAND(100) > 50 && prev_line_attr > 0
				? prev_line_attr - 1
				: prev_line_attr;*/
			
			WORD attr = RAND(100) > 50 && prev_line_attr > 0
				? (prev_line_attr - 1) & 0x0e
				: prev_line_attr & 0x0e;
			
			// if (attr <= 0x0d) attr &= 0x0d;
			if (attr < 0x0c) attr &= 4;

			console->attrs[index] = attr;
		}
	}
}
// =============================================================================
// @@@ + App_update
// =============================================================================
void App_update(App *app)
{
	Console *console = app->console;

	for (int row = console->height - 2; row >= 0; row--)
	{
		for (int col = 0; col < console->width; col++)
		{
			size_t index = row * console->width + col;
			
			WORD curr_line_attr = console->attrs[index];
			WORD prev_line_attr = console->attrs[index + console->width];
		
		    int value = RAND(100);

		    if (value > 70 && prev_line_attr)
		    {
		    	WORD attr = prev_line_attr;
		    	
		    	char offset = col ? RAND(3) - 1 : 0;

		    	// if (RAND(100) > 75) attr &= 0x0e;
		    	if (attr < 0x0c) attr &= 4;

		    	console->attrs[index + offset] = attr;
		    	continue;
		    }

		    if (value > 62 && curr_line_attr)
		    {
		    	WORD attr = (curr_line_attr - 1) & 0x0e;
		    	
		    	// if (attr <= 0x0d) attr &= 0x0d;
		    	if (attr < 0x0c) attr &= 4;

		    	console->attrs[index] = attr;
		    	continue;
		    }
		}
	}
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
	}
	return 1;
}

int main()
{
	system("cls");

	INIT_RAND;

	Console console = Console_create();
	Console_mem_fill(&console, "#", 0x0000);

	App app = {
		.console     = &console,
		.render_ones = 0,
	};

	App_first_pass(&app);
	Console_render_buff(&console);

	while (App_listen(&app))
	{
		// App_render(&console);
		
		App_update(&app);
		Console_render_attrs(&console);
		usleep(20000);
		// Sleep(40);
	}

	App_free(&app);


	// _getch();

	return 0;
}