#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <conio.h>
#include <windows.h>
#include <unistd.h>

#include "h/macros.h"

#define SEEK_OFFSET 17
#define DEFAULT_FB_SIZE 768
#define TARGET_FB_SIZE 3072
#define DEFAULT_CHAR_W 8
#define DEFAULT_CHAR_H 8
#define CHAR_W 16
#define CHAR_H 16
#define SCROLL_CHAR_WIDTH 8

#define BIT_MASK 0b1000000000000000
#define ON_BIT '#'
#define NO_BIT ':'
#define DELAY 33000

#define char2view(ascii) (uint16_t*)app->font_buff + ((ascii) - 32) * CHAR_H

#define CONSOLE_IMPLEMENTATION
#include "h/console.h"

typedef struct {
	char *start;
	char *end;

} Line;

typedef struct {
	Line      lines[CHAR_H];
	char     *current_char;
	uint16_t *current_char_view;
	uint16_t  current_char_mask;
	uint16_t  width;
	uint16_t  tail;
	uint16_t  size;

} View;

typedef struct {
	View  *view;
	Console *console;
	char *font_buff;
	char *message;

} App;

// =============================================================================
// @@@ + App_load_font
// =============================================================================
uint8_t App_load_font(App *app, const char *filename)
{
	FILE *fp = fopen(filename, "rb");

	if (!fp) return 0;

	fseek(fp, SEEK_OFFSET, SEEK_SET); // seek 17 bytes forward

	app->font_buff = malloc(TARGET_FB_SIZE);

	char byte = 0;

	for(uint16_t *p = (uint16_t*)app->font_buff; ; p += 2)
	{
		int byte = fgetc(fp);

		if (byte == EOF) break;

		*p = 0;

		for (int i = 0; i < DEFAULT_CHAR_W; i++)
		{
			char bit = (byte & 128) ? 1 : 0;

			*p <<= 1;
			*p |= bit;

			*p <<= 1;
			*p |= bit;
			
			byte <<= 1;
		}
		
		*(p + 1) = *p;
	}

	fclose(fp);

	return 1;
}


// =============================================================================
// @@@ + App_init
// =============================================================================
void App_init(App *app, const char *message)
{
	// puts("App_init");

	size_t str_len               = strlen(message);
	size_t scroll_width          = str_len < SCROLL_CHAR_WIDTH ? str_len * CHAR_W: SCROLL_CHAR_WIDTH * CHAR_W;
	size_t size                  = str_len * CHAR_W * CHAR_H;

	app->message                 = strdup(message);
	
	app->view->width             = scroll_width;
	app->view->tail              = 0;
	app->view->current_char      = &app->message[0];
	app->view->current_char_mask = BIT_MASK;
	app->view->current_char_view = char2view(message[0]);

	int screen_x                 = (app->console->width - app->view->width) >> 1;
	int screen_y                 = (app->console->height - CHAR_H) >> 1;

	View *view = app->view;
	
	//
	// Init view lines
	//
	for (int i = 0; i < CHAR_H; i++)
	{
		view->lines[i].start = app->console->buff + app->console->width * (screen_y + i) + screen_x;
		view->lines[i].end   = view->lines[i].start + scroll_width - 1;

		memset(view->lines[i].start, NO_BIT, scroll_width);
	}
}

// =============================================================================
// @@@ + App_update
// =============================================================================
void App_update(App *app)
{
	// puts("App_update");

	View *view = app->view;

	uint16_t width = view->width;
	uint16_t mask = view->current_char_mask;

	//
	// Move to next character
	//
	if (!mask)
	{
		view->current_char++;

		if (*view->current_char == 0)
			view->tail = view->width;

		view->current_char_mask = BIT_MASK;
		view->current_char_view = char2view(*view->current_char);
	}

	//
	// Scroll view one pixel left
	//
	for (int line_index = 0; line_index < CHAR_H; line_index++)
	{
		Line line = view->lines[line_index];

		memmove(
			line.start + 0,
			line.start + 1,
			width - 1
		);

		uint16_t bit = view->tail
			? 0
			: view->current_char_view[line_index] & mask;

		*line.end = bit ? ON_BIT : NO_BIT;
	}

	//
	// Decrease tail only in case if it is greater then 0
	// If tail become 0 then view->current_char looks one byte ahead of app->message
	//
	if (view->tail && --view->tail == 0)
		view->current_char = app->message - 1;

	view->current_char_mask >>= 1;
}

// =============================================================================
// @@@ + App_destroy
// =============================================================================
void App_destroy(App *app)
{
	Console_mem_free(app->console);

	if (app->font_buff)
		free(app->font_buff);

	if (app->message)
		free(app->message);

	app->font_buff   = NULL;
	app->message     = NULL;
}

// =============================================================================
// @@@ + App_exit
// =============================================================================
void App_exit(App *app, const char *message)
{
	perror(message);
	App_destroy(app);
	exit(1);
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
		console->width * CHAR_H,
		(COORD){0, 0},
		&console->written
	);
}

// =============================================================================
// @@@ + App_listen
// =============================================================================
uint8_t App_listen(App *app)
{
	// puts("App_listen");

	/*if (Console_check_resize(app->console))
		return App_resize(app);*/

	if (_kbhit())
	// if (1)
	{
		char key = _getch();

		if (key == 27 || ((key | 32) == 'q'))
			return 0;
	}
	return 1;
}

int main(int argc, const char *argv[])
{
	Console console = Console_create();
	Console_mem_fill(&console, " ", 0x07);

	View view = { 0 };

	App app = {
		.console   = &console,
		.view      = &view,
		.font_buff = NULL,
		.message   = NULL,
	};

	if (!App_load_font(&app, "DB_fnt.$C"))
		App_exit(&app, "File open Error");
	
	CURSOR_INIT(&app);
	CURSOR_HIDE(&app);

	// App_init(&app, "RunStr test message * ");
	// App_init(&app, "123");
	App_init(
		&app,
		argc == 2 ? argv[1] : "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum. "
	);
	Console_render(&console);
	// App_render(&app);
		
	while (App_listen(&app))
	{
		App_update(&app);
		Console_render_buff(&console);
		// App_render(&app);
		// Sleep(40);
		usleep(DELAY);
	}
	
	// App_print_font(&app);
	CURSOR_SHOW(&app);	

	App_destroy(&app);
	
	return 0;
}
