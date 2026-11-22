#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <conio.h>
#include <windows.h>
#include <unistd.h>

#include "h/macros.h"

#define SEEK_OFFSET 17
#define FB_SIZE 768
#define DEFAULT_CHAR_W 8
#define DEFAULT_CHAR_H 8
#define CHAR_W 16
#define CHAR_H 16
#define CHAR_SCALE 1
#define SCROLL_CHAR_WIDTH 8

#define CONSOLE_IMPLEMENTATION
#include "h/console.h"

typedef struct {
	char *buff;
	char *buff_lines[CHAR_H];
	uint32_t line_width;
	uint32_t line_offset;
	uint32_t size;
	uint16_t scroll_width;

} View;

typedef struct {
	Console *console;
	View *source_view;
	View *target_view;
	char *font_buff;
	char *view_buff;
	char *message;

} App;

void App_update(App *app);

// =============================================================================
// @@@ + App_load_font
// =============================================================================
bool App_load_font(App *app, const char *filename)
{
	FILE *fp = fopen(filename, "rb");

	if (!fp) return 0;

	fseek(fp, SEEK_OFFSET, SEEK_SET); // seek 17 bytes forward

	app->font_buff = malloc(FB_SIZE);

	bool res = fread(app->font_buff, sizeof(char), FB_SIZE, fp) == FB_SIZE;

	fclose(fp);

	return res;
}

// =============================================================================
// @@@ + App_init
// =============================================================================
void App_init(App *app, const char *message)
{
	// puts("App_init");

	size_t str_len                 = strlen(message);
	size_t scroll_width            = str_len < SCROLL_CHAR_WIDTH ? str_len * CHAR_W: SCROLL_CHAR_WIDTH * CHAR_W;
	size_t size                    = str_len * CHAR_W * CHAR_H;

	app->message                   = strdup(message);
	app->view_buff                 = malloc(size);

	app->source_view->buff         = app->view_buff;
	app->source_view->line_width   = str_len * CHAR_W;
	app->source_view->line_offset  = 0;
	app->source_view->size         = size;
	app->source_view->scroll_width = scroll_width;

	app->target_view->buff         = app->console->buff;
	app->target_view->line_width   = app->console->width;
	app->target_view->size         = app->console->size;
	app->target_view->line_offset  = 0;
	app->target_view->scroll_width = scroll_width;

	int screen_x = (app->console->width - app->source_view->scroll_width) >> 1;
	int screen_y = (app->console->height - CHAR_H) >> 1;

	char *p_view = app->view_buff;

	for (int char_line_index = 0, view_line_index = 0; char_line_index < DEFAULT_CHAR_H; char_line_index++)
	{
		app->source_view->buff_lines[view_line_index] = app->source_view->buff + app->source_view->line_width * view_line_index;
		app->target_view->buff_lines[view_line_index] = app->target_view->buff + app->target_view->line_width * (view_line_index + screen_y) + screen_x;

		//
		// Compose a line
		//
		for (char *char_ptr = app->message; *char_ptr; char_ptr++)
		{
			char *letter = app->font_buff + (*char_ptr - 32) * DEFAULT_CHAR_H;
			char byte = letter[char_line_index];

			for (int i = 0; i < DEFAULT_CHAR_W; i++)
			{
				char bit = (byte & 128) ? '#' : '.';
				
				*p_view++ = bit;
				*p_view++ = bit;
				
				byte <<= 1;
			}
		}

		//
		// Duplicate prev line
		//
		view_line_index++;

		app->source_view->buff_lines[view_line_index] = app->source_view->buff + app->source_view->line_width * view_line_index;
		app->target_view->buff_lines[view_line_index] = app->target_view->buff + app->target_view->line_width * (view_line_index + screen_y) + screen_x;

		memcpy(
			app->source_view->buff_lines[view_line_index - 0],
			app->source_view->buff_lines[view_line_index - 1],
			app->source_view->line_width
		);

		p_view += app->source_view->line_width;

		view_line_index++;
	}
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
// @@@ + App_print
// =============================================================================
void App_print(App *app)
{
	char str[9] = {0};

	for (int index = 0; index < FB_SIZE; index++)
	{
		for (int i = 0, mask = 0b10000000; mask; i++)
		{
			str[i] = app->font_buff[index] & mask ? '#' : ' ';
			mask >>= 1;
		}
		puts(str);
	}
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

int main()
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

	CURSOR_INIT(&app);
	CURSOR_HIDE(&app);

	App_init(&app, "RunStr test message * ");
	// App_init(&app, "R * ");
	Console_render(&console);
	App_render(&app);

	while (App_listen(&app))
	{
		App_update(&app);
		Console_render_buff(&console);
		// App_render(&app);
		// Sleep(40);
		usleep(40000);
	}

	App_destroy(&app);
	CURSOR_SHOW(&app);

	return 0;
}
