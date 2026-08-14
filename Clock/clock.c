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

	char *dest_chars = console->buff  + frame_screen_x + frame_screen_y * console->width;
	WORD *dest_attrs = console->attrs + frame_screen_x + frame_screen_y * console->width;
	
	for (int line = 0; line < frame_height; line++)
	{
		PUSH_ADDR(dest_chars);

		if (line == first_row) {
			dest_chars[0]               = 'Ú';
			dest_chars[frame_width - 1] = '¿';
		}
		else if (line == final_row) {
			dest_chars[0]               = 'À';
			dest_chars[frame_width - 1] = 'Ù';
		}
		else {
			dest_chars[0]               = '³';
			dest_chars[frame_width - 1] = '³';
		}

		for (int x = 0; x < frame_width - 2; x++) {
			dest_chars[x + 1] = (!line || line == frame_height - 1) ? 'Ä' : ' ';
		}

		POP_ADDR(dest_chars);
		INC_LINE(dest_chars);

		// Colorize border with border color
		//----------------------------------
		void *tmp_attrs = dest_attrs;

		for (int i = 0; i < frame_width; i++) {
			dest_attrs[i] = clock->frame_color;
		}

		dest_attrs = tmp_attrs;
		dest_attrs += console->width;
	}

	clock->draw_frame = NULL;
}

// =============================================================================
// @@@ + Clock_set_digits_color
//
// !!! This function will be called when the program starts,
// !!! or after charset is changed
//
// =============================================================================
void Clock_set_digits_color(Clock *clock)
{
	Console *console = clock->console;
	Charset *charset = clock->charset;

	const char *str_time = clock->timebuff;

	int screenx = (console->width  - charset->cell_w * strlen(str_time)) >> 1;
	int screeny = (console->height - charset->cell_h) >> 1;

	WORD *dest = console->attrs + screeny * console->width + screenx;

	for (int line = 0; line < charset->cell_h; line++)
	{
		PUSH_ADDR(dest);

		int length = strlen(str_time) * charset->cell_w;

		for (int i = 0; i < length; i++) {
			dest[i] = clock->clock_color;
		}

		POP_ADDR(dest);
		INC_LINE(dest);
	}
}

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
// @@@ + Clock_print
// =============================================================================
void Clock_print(Clock *clock)
{
	Console *console = clock->console;
	Charset *charset = clock->charset;

	void *target_chars = clock->dialbox.target_chars;
	
	for (int line = 0; line < charset->cell_h; line++)
	{
		PUSH_ADDR(target_chars);

		for (const char *p = clock->timebuff; *p; p++)
		{
			int index = *p - 47;

			memcpy(
				target_chars,
				charset->data[index][line],
				charset->cell_w
			);
			target_chars += charset->cell_w;
		}

		POP_ADDR(target_chars);
		INC_LINE(target_chars);
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
// @@@ + Clock_get_bounds
// =============================================================================
void Clock_get_bounds(Clock *clock)
{
	Clock_get_time(clock);

	Console *console = clock->console;

	Bounds *dialbox  = &clock->dialbox;
	Bounds *framebox = &clock->framebox;

	// FRAME
	framebox->width  = clock->padding_x * 2 + strlen(clock->timebuff) * clock->charset->cell_w;
	framebox->height = clock->padding_y * 2 + clock->charset->cell_h;
	framebox->x      = (console->width  - framebox->width)  >> 1;
	framebox->y      = (console->height - framebox->height) >> 1;

	framebox->target_chars = console->buff  + framebox->x + framebox->y * console->width;
	framebox->target_attrs = console->attrs + framebox->x + framebox->y * console->width;

	// DIGITS
	dialbox->x = (console->width  - clock->charset->cell_w * strlen(clock->timebuff)) >> 1;
	dialbox->y = (console->height - clock->charset->cell_h) >> 1;

	dialbox->target_chars = console->buff  + dialbox->y * console->width + dialbox->x;
	dialbox->target_attrs = console->attrs + dialbox->y * console->width + dialbox->x;
}

// =============================================================================
// @@@ + Clock_trigger_update
// =============================================================================
void Clock_trigger_update(Clock *clock)
{
	Console_fill_buffs(clock->console);
	Clock_get_bounds(clock);

	if (clock->has_frame)
		Clock_draw_frame(clock);
	
	Clock_set_digits_color(clock);

	clock->trigger_update = NULL;
}

// =============================================================================
// @@@ + app_listen
// =============================================================================
uint16_t App_listen(Clock *clock)
{
	if (Console_check_resize(clock->console))
	{
		Console_reset(clock->console);
		Clock_trigger_update(clock);
		return 1;
	}

	if (_kbhit())
	{
		char key = _getch();

		if (((key | 32) == 'q' || key == 27))
			return 0;

		if ((key | 32) == 'f')
		{
			clock->has_frame ^= 1;
			Clock_trigger_update(clock);

			return 1;
		}

		if (key >= '1' && key <= '9')
		{
			int index = (key - 49) % clock->charset_list_size;
			clock->charset = &clock->charset_list[index];

			Clock_trigger_update(clock);
		}
	}

	return 1;
}

int main()
{
	system("cls"); // required to be able to work in Conemu

	Charset charset_list[] = {
		charset01,
		charset02,
		charset03,
		charset05,
		charset_lines,
		charset_electronika,
	};

	Console console = Console_create();
	Console_fill_buffs(&console);

	Clock clock = {
		.console           = &console,
		.charset           = &charset_list[2],
		.charset_list      = charset_list,
		.charset_list_size = sizeof(charset_list) / sizeof(*charset_list),
		.padding_x         = 6,
		.padding_y         = 3,
		.has_frame         = 1,
		.clock_color       = 0x07,
		.frame_color       = 0x08,
		.trigger_update    = Clock_trigger_update,
		.dialbox           = { 0 },
		.framebox          = { 0 },
	};

	Clock_get_bounds(&clock);

	while (App_listen(&clock))
	{
		Clock_get_time(&clock);

		if (clock.trigger_update)
			clock.trigger_update(&clock);

		Clock_print(&clock);
		Sleep(50);
	}
	
	Console_free(&console);

	return 0;
}