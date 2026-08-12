#include <stdio.h>
#include <stdint.h>
#include <conio.h>
#include <string.h>
#include <windows.h>
#include <time.h>

#define CONSOLE_IMPLEMENTATION
#include "h/console.h"

#include "h/clock.h"
#include "h/charset.h"

#define CHARSET_01_IMPLEMENTATION
	#include "h/charset01.h"

#define CHARSET_02_IMPLEMENTATION
	#include "h/charset02.h"

#define CHARSET_03_IMPLEMENTATION
	#include "h/charset03.h"

#define CHARSET_05_IMPLEMENTATION
	#include "h/charset05.h"

#define CHARSET_LINES_IMPLEMENTATION
	#include "h/charset-lines.h"

#define CHARSET_ELECTRONOCA_IMPLEMENTATION
	#include "h/charset-electronika-01.h"

#define PUSH_ADDR(addr) void *tmp = (addr);
#define  POP_ADDR(addr) (addr) = tmp;
#define  INC_LINE(addr) (addr) += console->width;

// =============================================================================
// @@@ + Clock_render
// =============================================================================
void Clock_render(Console *console)
{
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

// =============================================================================
// @@@ + Clock_draw_frame
// =============================================================================
void Clock_draw_frame(Clock *clock)
{
	Console *console = clock->console;

	uint16_t frame_width  = clock->padding_x * 2 + strlen(clock->timebuff) * clock->charset->cell_w;
	uint16_t frame_height = clock->padding_y * 2 + clock->charset->cell_h;

	uint16_t frame_screen_x = (console->width  - frame_width)  >> 1;
	uint16_t frame_screen_y = (console->height - frame_height) >> 1;

	int first_row = 0;
	int final_row = frame_height - 1;

	char *dest = console->buff + frame_screen_x + frame_screen_y * console->width;
	
	for (int line = 0; line < frame_height; line++)
	{
		PUSH_ADDR(dest);

		if (line == first_row) {
			dest[0]                = 'Ú';
			dest[frame_width - 1] = '¿';
		}
		else if (line == final_row) {
			dest[0]                = 'À';
			dest[frame_width - 1] = 'Ù';
		}
		else {
			dest[0]                = '³';
			dest[frame_width - 1] = '³';
		}

		for (int x = 0; x < frame_width - 2; x++) {
			dest[x + 1] = (!line || line == frame_height - 1) ? 'Ä' : ' ';
		}

		POP_ADDR(dest);
		INC_LINE(dest);
	}
}

// =============================================================================
// @@@ + Clock_print
// =============================================================================
void Clock_print(Clock *clock)
{
	Console *console = clock->console;
	Charset *charset = clock->charset;
	
	const char *str_time = clock->timebuff;

	int screenx = (console->width  - charset->cell_w * strlen(str_time)) >> 1;
	int screeny = (console->height - charset->cell_h) >> 1;

	char *dest = console->buff + screeny * console->width + screenx;

	for (int line = 0; line < charset->cell_h; line++)
	{
		PUSH_ADDR(dest);

		for (const char *p = str_time; *p; p++)
		{
			int index = *p - 47;
			memcpy(dest, charset->data[index][line], charset->cell_w);
			dest += charset->cell_w;
		}

		POP_ADDR(dest);
		INC_LINE(dest);
	}

	Clock_render(console);
}

// =============================================================================
// @@@ + Clock_get_time
// =============================================================================
void Clock_get_time(Clock *clock)
{
	time_t now = time(NULL);
	localtime_s(&clock->time, &now);

	sprintf(clock->timebuff, "%02d:%02d:%02d",
		clock->time.tm_hour,
		clock->time.tm_min,
		clock->time.tm_sec
	);
}

// =============================================================================
// @@@ + app_listen
// =============================================================================
uint16_t App_listen(Clock *clock)
{
	if (_kbhit())
	{
		char key = _getch();
		if (key == 27 || ((key | 32) == 'q')) return 0;

		if (key >= '1' && key <= '9')
		{
			Console_clear_buff(clock->console);

			int index = (key - 49) % clock->charset_list_size;
			clock->charset = &clock->charset_list[index];
		}
	}

	return 1;
}

int main()
{
	Charset charset_list[] = {
		charset01,
		charset02,
		charset03,
		charset05,
		charset_lines,
		charset_electronika,
	};

	const char *time_format = "##:##:##";

	Console console = Console_create();
	Console_clear_buff(&console);

	Clock clock = {
		.console           = &console,
		.charset           = &charset_list[2],
		.charset_list      = charset_list,
		.charset_list_size = sizeof(charset_list) / sizeof(*charset_list),
		.padding_x         = 6,
		.padding_y         = 3,
	};
	
	while (App_listen(&clock))
	{
		Clock_get_time(&clock);
		Clock_draw_frame(&clock);
		Clock_print(&clock);
		Sleep(50);
	}
	// printf("%d\n", sizeof(charsets) / sizeof(*charsets));
	
	Console_free(&console);

	return 0;
}