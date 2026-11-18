#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <conio.h>
#include <windows.h>

#define SEEK_OFFSET 17
#define FB_SIZE 768
#define CHAR_W 8
#define CHAR_H 8

#define CONSOLE_IMPLEMENTATION
#include "h/console.h"

typedef struct {
	char *buff;
	char *lines[CHAR_H];
	uint32_t width;
	uint32_t size;
	uint32_t offset;

} View;

typedef struct {
	Console *console;
	View *source_view;
	View *target_view;
	char *font_buff;
	char *message;

} App;

void App_update_frame(App *app);

// =============================================================================
// @@@ + View_destroy
// =============================================================================
void View_destroy(View *view)
{
	if (view->buff)
		free(view->buff);

	view->buff = NULL;
}

// =============================================================================
// @@@ + View_init
// =============================================================================
void* View_init(View *view, const char *message)
{
	size_t str_len = strlen(message);

    view->width = str_len * CHAR_W;
    view->size  = str_len * CHAR_W * CHAR_H;
	view->buff  = malloc(view->size);

	return view->buff;
}

// =============================================================================
// @@@ + App_init
// =============================================================================
void App_init(App *app, const char *message)
{
	puts("App_init");

	size_t str_size = strlen(message);
	size_t index = 0;

	app->message = malloc(sizeof(message));
	strcpy(app->message, message);
	// app->view_buff = malloc(strlen(message) * CHAR_W * CHAR_H);

	char *p_view = View_init(app->source_view, message);

	for (int line = 0; line < CHAR_H; line++)
	{
		for (char *char_ptr = app->message; *char_ptr; char_ptr++)
		{
			char *letter = app->font_buff + (*char_ptr - 32) * CHAR_H;
			char byte = letter[line];

			for (int i = 0; i < CHAR_W; i++)
			{
				*p_view++ = (byte & 128) ? '#' : ' ';
				byte <<= 1;
			}
		}
	}
	// App_update(app);
	App_update_frame(app);
	Console_render(app->console);
}

// =============================================================================
// @@@ + App_load_font
// =============================================================================
uint8_t App_load_font(App *app, const char *filename)
{
	FILE *fp = fopen(filename, "rb");

	if (!fp) return 0;

	fseek(fp, SEEK_OFFSET, SEEK_SET); // seek 17 bytes forward

	app->font_buff = malloc(FB_SIZE);

	uint8_t res = fread(app->font_buff, sizeof(char), FB_SIZE, fp) == FB_SIZE;

	fclose(fp);

	return res;
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
// @@@ + App_free
// =============================================================================
void App_free(App *app)
{
	Console_mem_free(app->console);

	if (app->font_buff)
		free(app->font_buff);

	if (app->message)
		free(app->message);

	if (app->source_view)
		View_destroy(app->source_view);

	app->font_buff   = NULL;
	app->message     = NULL;
	app->source_view = NULL;
	app->target_view = NULL;
}

// =============================================================================
// @@@ + App_update_frame
// =============================================================================
void App_update_frame(App *app)
{
	puts("App_update_frame");

	int str_size = strlen(app->message);

	for (int line = 0; line < 8; line++)
	{
		char *dest = (char*)app->console->buff + line * app->console->width;
		char *src = app->source_view->buff + line * str_size * CHAR_W;
		memcpy(dest, src, str_size * 8);
	}
}

// =============================================================================
// @@@ + App_exit
// =============================================================================
void App_exit(App *app, const char *message)
{
	perror(message);
	App_free(app);
	exit(1);
}

int main()
{
	system("cls");

	Console console = Console_create();
	Console_mem_fill(&console, " ", 0x07);

	View source_view = {
		.buff   = NULL,
		.width  = 0,
		.size   = 0,
		.offset = 0,
	};

	View target_view = {
		.buff   = console.buff,
		.width  = console.width,
		.size   = console.width * CHAR_H,
		.offset = 0,
	};

	App app = {
		.console     = &console,
		.source_view = &source_view,
		.target_view = &target_view,
		.font_buff   = NULL,
		.message     = NULL,
	};

	if (!App_load_font(&app, "DB_fnt.$C"))
		App_exit(&app, "File open Error");

	App_init(&app, "RunStr test message");
	// App_print(&app);

	// printf("%zu", sizeof(View));

	// _getch();

	return 0;
}
