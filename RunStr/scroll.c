#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <conio.h>
#include <windows.h>
#include <unistd.h>

#include "h/macros.h"

#define SEEK_OFFSET 17
#define DEFAULT_FB_SIZE 768
#define  TARGET_FB_SIZE 3072
#define DEFAULT_CHAR_W 8
#define DEFAULT_CHAR_H 8
#define CHAR_W 16
#define CHAR_H 16
#define CHAR_SCALE 1
#define SCROLL_CHAR_WIDTH 8

#define ON_BIT '#'
#define NO_BIT '.'
#define DELAY 33000

#define CONSOLE_IMPLEMENTATION
#include "h/console.h"
/*
typedef struct {
	char *buff;
	char *buff_lines[CHAR_H];
	uint32_t line_width;
	uint32_t line_offset;
	uint32_t size;
	uint16_t scroll_width;

} View;
*/
typedef struct {
	char    *lines[CHAR_H];
	char    *current_char;
	uint16_t current_char_mask;
	uint16_t width;
	uint16_t size;

} View;
/*
typedef struct {
	Console *console;
	View *source_view;
	View *target_view;
	char *font_buff;
	char *view_buff;
	char *message;

} App;
*/
typedef struct {
	Console *console;
	View *view;
	char *font_buff;
	char *message;

} App;

void App_update(App *app);

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

		if (byte == EOF)
			break;

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

	size_t str_len      = strlen(message);
	size_t scroll_width = str_len < SCROLL_CHAR_WIDTH ? str_len * CHAR_W: SCROLL_CHAR_WIDTH * CHAR_W;
	size_t size         = str_len * CHAR_W * CHAR_H;

	app->message        = strdup(message);

	app->view->width    = scroll_width;
	app->view->size     = app->console->size;

	int screen_x = (app->console->width - app->view->width) >> 1;
	int screen_y = (app->console->height - CHAR_H) >> 1;

	for (int i = 0; i < CHAR_H; i++)
		view->lines[i] =  screen_x + app->console->width * i;
}

// =============================================================================
// @@@ + App_update
// =============================================================================
void App_update(App *app)
{
	// puts("App_update");

	int str_size = strlen(app->message);

	size_t scroll_width = app->source_view->scroll_width;
	size_t width        = app->source_view->line_width;
	size_t offset       = app->source_view->line_offset;
	size_t fraction     = width - offset;

	size_t len1 = scroll_width < fraction ? scroll_width: fraction;
	size_t len2 = scroll_width - fraction;

	for (int line_index = 0; line_index < CHAR_H; line_index++)
	{
		memcpy(
			app->target_view->buff_lines[line_index] + 0,
			app->source_view->buff_lines[line_index] + offset,
			len1
		);

		if (fraction < scroll_width)
			memcpy(
				app->target_view->buff_lines[line_index] + fraction,
				app->source_view->buff_lines[line_index] + 0,
				len2
			);
	}
	
	app->source_view->line_offset++;

	if (app->source_view->line_offset == app->source_view->line_width)
		app->source_view->line_offset = 0;
}

// =============================================================================
// @@@ + App_destroy
// =============================================================================
void App_destroy(App *app)
{
	Console_mem_free(app->console);

	if (app->font_buff)
		free(app->font_buff);

	if (app->view_buff)
		free(app->view_buff);

	if (app->message)
		free(app->message);

	if (app->source_view)
		free(app->source_view);

	if (app->target_view)
		free(app->target_view);

	app->font_buff   = NULL;
	app->view_buff   = NULL;
	app->message     = NULL;
	app->source_view = NULL;
	app->target_view = NULL;
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
	// system("cls");

	Console console = Console_create();
	Console_mem_fill(&console, " ", 0x07);

	App app = {
		.console     = &console,
		.source_view = malloc(sizeof(View)),
		.target_view = malloc(sizeof(View)),
		.font_buff   = NULL,
		.view_buff   = NULL,
		.message     = NULL,
	};

	if (!App_load_font(&app, "DB_fnt.$C"))
		App_exit(&app, "File open Error");
	/*
	CURSOR_INIT(&app);
	CURSOR_HIDE(&app);

	// App_init(&app, "RunStr test message * ");

	App_init(
		&app,
		argc == 2 ? argv[1] : "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum. "
	);
	Console_render(&console);
	App_render(&app);

	while (App_listen(&app))
	{
		App_update(&app);
		Console_render_buff(&console);
		// App_render(&app);
		// Sleep(40);
		usleep(DELAY);
	}

	CURSOR_SHOW(&app);
	*/

	App_destroy(&app);
	
	return 0;
}
