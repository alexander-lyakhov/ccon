#include <stdio.h>
#include <stdint.h>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include <unistd.h>

#define ATTR_BLACK 0x00
#define ATTR_WHITE 0x0F
#define ATTR_GREEN 0x0A

#define CURSOR_INIT(app) CONSOLE_CURSOR_INFO cursorInfo; GetConsoleCursorInfo((app)->console->handle, &cursorInfo);
#define CURSOR_HIDE(app) cursorInfo.bVisible = 0; SetConsoleCursorInfo((app)->console->handle, &cursorInfo);
#define CURSOR_SHOW(app) cursorInfo.bVisible = 1; SetConsoleCursorInfo((app)->console->handle, &cursorInfo);

typedef struct {
	uint16_t count;
	WORD attr;

} Drop;

typedef struct {
	HANDLE handle;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD written;

	char* buff;
	WORD* attrs;

	uint16_t width;
	uint16_t height;
	uint16_t size;

} Console;

typedef struct {
	Console *console;
	Drop *drops;
	uint32_t delay;
	uint8_t  depth;

} App;

Console* Console_init_buffs(Console *console);
void App_skip_begin(App *app);

// ================================================================================
// @@@ + Drop_create
// ================================================================================
Drop Drop_create(App *app)
{
	uint8_t count = (rand() % app->console->height >> 1) + 3; // min length

	WORD attr = (rand() % 100) > 75 ? ATTR_GREEN : ATTR_BLACK;

	if (app->depth && attr != ATTR_BLACK)
	{
		if ((rand() % 10) > 3)
			attr &= 0b0111;
	}

	return (Drop) {
		.count = count,
		.attr = attr
	};
}

// ================================================================================
// @@@ + Console_create
// ================================================================================
Console Console_create()
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	GetConsoleScreenBufferInfo(handle, &csbi);

	uint16_t width  = csbi.srWindow.Right + 1;
	uint16_t height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
	uint16_t size   = width * height;

	return (Console) {
		.handle   = handle,
		.width    = width,
		.height   = height,
		.size     = size,
		.buff     = NULL,
		.attrs    = NULL,
	};
}

// =============================================================================
// @@@ + Console_new
// =============================================================================
Console* Console_new()
{
	Console *console = malloc(sizeof(Console));
	Console con = Console_create();

	memcpy(console, &con, sizeof(Console));
	Console_init_buffs(console);

	return console;
}

// =============================================================================
// @@@ + Console_init_buffs
// =============================================================================
Console* Console_init_buffs(Console *console)
{
	console->buff  = malloc(console->size * sizeof(char));
	console->attrs = malloc(console->size * sizeof(WORD));

	for (int i = 0; i < console->size; i++)
		console->buff[i] = rand() % 94 + 32;

	memset(
		console->attrs,
		ATTR_BLACK,
		console->size * sizeof(*console->attrs)
	);

	return console;
}

// =============================================================================
// @@@ + Console_free_buffs
// =============================================================================
void Console_free_buffs(Console *console)
{
	free(console->buff);
	free(console->attrs);

	console->buff  = NULL;
	console->attrs = NULL;
}

// =============================================================================
// @@@ + App_init_drops
// =============================================================================
Drop* App_init_drops(App *app)
{
	Console *console = app->console;

	Drop *drops = malloc(sizeof(Drop) * console->width);

	for (int i = 0; i < console->width; i++)
	{
		drops[i] = Drop_create(app);
		console->attrs[i] = ATTR_WHITE; // first top line is always WHITE
	}

	return drops;
}

// =============================================================================
// @@@ + App_create
// =============================================================================
App App_create()
{
	srand(time(NULL));

	Console *console = Console_new();

	App app;

	app.console = console;
	app.delay   = 40000;
	app.depth   = 1;
	app.drops   = App_init_drops(&app);

	return app;
}

// =============================================================================
// @@@ + App_reset
// =============================================================================
void App_reset(App *app)
{
	free(app->drops);
	free(app->console->buff);
	free(app->console->attrs);

	system("cls");

	Console_init_buffs(app->console);
	app->drops = App_init_drops(app);

	CURSOR_INIT(app);
	CURSOR_HIDE(app);

	App_skip_begin(app);
}

// =============================================================================
// @@@ + App_destroy
// =============================================================================
void App_destroy(App *app)
{
	Console_free_buffs(app->console);

	free(app->drops);
	free(app->console);
}

// =============================================================================
// @@@ + App_check_resize
// =============================================================================
uint8_t App_check_resize(App *app)
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	GetConsoleScreenBufferInfo(handle, &csbi);

	uint16_t width  = csbi.srWindow.Right  + 1;
	uint16_t height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

	if (app->console->width != width || app->console->height != height)
	{
		app->console->width  = width;
		app->console->height = height;
		app->console->size   = width * height;

		App_reset(app);

		return 1;
	}

	return 0;
}

// =============================================================================
// @@@ + App_listen
// =============================================================================
uint8_t App_listen(App *app)
{
	if (App_check_resize(app)) return 1;

	if (kbhit())
	{
		int key = getch();

		if (key == 27 || (key | 32) == 'q')
			return 0;

		if (key >= '1' && key <= '9') 
			app->delay = 10000 * (key & 0x0f);

		if ((key | 32) == 'b')
			app->depth ^= 1;
	}
	return 1;
}

// =============================================================================
// @@@ + App_update
// =============================================================================
void App_update(App *app)
{
	Console *console = app->console;

	size_t trg_index = console->size - console->width;
	size_t src_index = trg_index - console->width;

	for (int i = console->height - 1; i > 0; i--)
	{
		memcpy(
			&console->attrs[trg_index],
			&console->attrs[src_index],
			console->width * sizeof(*console->attrs)
		);

		trg_index -= console->width;
		src_index -= console->width;
	}
		
	//
	// Very first top line
	//
	for (int i = 0; i < console->width; i++)
	{
		if (!app->drops[i].count--) {
			app->drops[i] = Drop_create(app);
		}
		else
		{
			console->attrs[i] = app->drops[i].attr;

			if (console->attrs[i] != ATTR_BLACK && console->attrs[i + console->width] == ATTR_BLACK)
			{
				console->attrs[i] = console->attrs[i] & 0b1000 // check if current attr is bright
					? ATTR_WHITE
					: ATTR_WHITE & 0b0111;
			}
		}
	}
}

// =============================================================================
// @@@ + App_render
// =============================================================================
void App_render(App *app)
{
	Console *console = app->console;

	WriteConsoleOutputCharacter(
		console->handle,
		console->buff,
		console->size,
		(COORD){0, 0},
		&console->written
	);

	WriteConsoleOutputAttribute(
		console->handle,
		console->attrs,
		console->size,
		(COORD){0, 0},
		&console->written
	);
}

// ================================================================================
// @@@ + App_skip_begin
// ================================================================================
void App_skip_begin(App *app)
{
	for (int row = 0; row < app->console->height - 1; row++)
		App_update(app);
}

int main()
{
	system("cls");

	App app = App_create();

	CURSOR_INIT(&app);
	CURSOR_HIDE(&app);

	App_skip_begin(&app);

	while (App_listen(&app))
	{
		App_render(&app);
		App_update(&app);

		// Sleep(30);
		usleep(app.delay);
	}
	
	App_destroy(&app);

	CURSOR_SHOW(&app);

	return 0;
}