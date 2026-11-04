#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#include <unistd.h>

#define SEEK_OFFSET 17
#define FB_SIZE 768
#define CHAR_W 8
#define CHAR_H 8

#define CONSOLE_IMPLEMENTATION
#include "h/console.h"

typedef struct {
	Console *console;
	char *font_buff;
	char *view_buff;
	char *message;

} App;


// =============================================================================
// @@@ + App_free
// =============================================================================
void App_free(App *app)
{
	Console_mem_free(app->console);

	if (app->font_buff)
		free(app->font_buff);

	if (app->view_buff)
		free(app->view_buff);

	if (app->message)
		free(app->message);

	app->font_buff = NULL;
	app->view_buff = NULL;
	app->message   = NULL;
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
// @@@ + App_exit
// =============================================================================
void App_exit(App *app, const char *message)
{
	perror(message);
	App_free(app);
	exit(1);
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
// @@@ + App_update_frame
// =============================================================================
void App_update_frame(App *app)
{
	int str_size = strlen(app->message);

	for (int line = 0; line < 8; line++)
	{
		char *dest = (char*)app->console->buff + line * app->console->width;
		char *src = app->view_buff + line * str_size * CHAR_W;
		memcpy(dest, src, str_size * 8);
	}
}


// =============================================================================
// @@@ + App_scroll
// =============================================================================
void App_scroll(App *app)
{
	Console *console = app->console;

	for (int line = 0; line < CHAR_H; line++)
	{
		char *dest = (char*)console->buff + line * console->width;

		for (int i = 0; i < console->width - 1; i++)
		{
			// char *dest = &console->buff[index];
			*dest = *(dest + 1);
			dest++;
		}
	}
}

// =============================================================================
// @@@ + App_update
// =============================================================================
void App_update(App *app)
{
	// App_update_frame(app);
	App_scroll(app);
}

// =============================================================================
// @@@ + App_init_view
// =============================================================================
void App_init_view(App *app, const char *message)
{
	size_t str_size = strlen(message);
	size_t index = 0;

	app->message   = malloc(sizeof(message));
	app->view_buff = malloc(strlen(message) * CHAR_W * CHAR_H);

	strcpy(app->message, message);

	char *p_view = app->view_buff;

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
// @@@ + App_listen
// =============================================================================
uint8_t App_listen(App *app)
{
	/*if (Console_check_resize(app->console))
		return App_resize(app);*/

	// if (_kbhit())
	if (1)
	{
		char key = _getch();

		if (key == 27 || ((key | 32) == 'q'))
			return 0;
	}
	return 1;
}

int main()
{
	system("cls");

	Console console = Console_create();
	Console_mem_fill(&console, " ", 0x07);

	App app = {
		.console   = &console,
		.font_buff = NULL,
		.font_buff = NULL,
		.message   = NULL,
	};

	if (!App_load_font(&app, "DB_fnt.$C"))
		App_exit(&app, "File open Error");

	App_init_view(&app, "RunStr test message");

	while (App_listen(&app))
	{
		App_update(&app);
		Console_render_buff(&console);
		usleep(20000);
		// Sleep(40);
	}

	// App_print(&app);

	App_free(&app);

	return 0;
}