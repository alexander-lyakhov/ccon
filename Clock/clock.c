#include <stdio.h>
#include <stdint.h>
#include <conio.h>
#include <string.h>
#include <windows.h>
#include <time.h>
#include <math.h>

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

	uint16_t frame_width  = clock->framebox.width;
	uint16_t frame_height = clock->framebox.height;

	uint16_t first_row = 0;
	uint16_t final_row = frame_height - 1;

	char *target_chars = clock->framebox.target_chars;
	WORD *target_attrs = clock->framebox.target_attrs;

	for (int line = 0; line < frame_height; line++)
	{
		PUSH_ADDR(target_chars);

		if (line == first_row) {
			target_chars[0]               = 'Ú';
			target_chars[frame_width - 1] = '¿';
		}
		else if (line == final_row) {
			target_chars[0]               = 'À';
			target_chars[frame_width - 1] = 'Ù';
		}
		else {
			target_chars[0]               = '³';
			target_chars[frame_width - 1] = '³';
		}

		for (int x = 0; x < frame_width - 2; x++) {
			target_chars[x + 1] = (!line || line == frame_height - 1) ? 'Ä' : ' ';
		}

		POP_ADDR(target_chars);
		INC_LINE(target_chars);
	}
}

// =============================================================================
// @@@ + Clock_apply_colors
// =============================================================================
void Clock_apply_colors(Clock *clock)
{
	Console *console = clock->console;

	// FRAME COLOR

	WORD *frame_target_attrs = clock->framebox.target_attrs;

	for (int line = 0; line < clock->framebox.height; line++)
	{
		PUSH_ADDR(frame_target_attrs);

		for (int i = 0; i < clock->framebox.width; i++) {
			frame_target_attrs[i] = clock->frame_color;
		}

		POP_ADDR(frame_target_attrs);
		INC_LINE(frame_target_attrs);
	}

	// DIGIT COLOR

	WORD *dial_target_attrs = clock->dialbox.target_attrs;

	for (int line = 0; line < clock->dialbox.height; line++)
	{
		PUSH_ADDR(dial_target_attrs);

		for (int i = 0; i < clock->dialbox.width; i++) {
			dial_target_attrs[i] = clock->clock_color;
		}

		POP_ADDR(dial_target_attrs);
		INC_LINE(dial_target_attrs);
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
// @@@ + Clock_get_time_full
// =============================================================================
void Clock_get_time_full(Clock *clock)
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
// @@@ + Clock_get_time_short
// =============================================================================
void Clock_get_time_short(Clock *clock)
{
	time_t now = time(NULL);
	localtime_s(&clock->time, &now);

	sprintf(clock->timebuff, "%02d:%02d",
		clock->time.tm_hour,
		clock->time.tm_min
	);
}

// =============================================================================
// @@@ + Clock_get_bounds
// =============================================================================
void Clock_get_bounds(Clock *clock)
{
	clock->get_time(clock);

	Console *console = clock->console;

	Bounds *dialbox  = &clock->dialbox;
	Bounds *framebox = &clock->framebox;

	// FRAME
	framebox->width  = fmin(clock->padding_x * 2 + strlen(clock->timebuff) * clock->charset->cell_w, console->width);
	framebox->height = fmin(clock->padding_y * 2 + clock->charset->cell_h, console->height);

	framebox->x = (console->width  - framebox->width)  >> 1;
	framebox->y = (console->height - framebox->height) >> 1;

	framebox->target_chars = console->buff  + framebox->x + framebox->y * console->width;
	framebox->target_attrs = console->attrs + framebox->x + framebox->y * console->width;

	if (framebox->width >= console->width || framebox->height >= console->height)
		clock->has_frame = 0;

	// DIGITS
	dialbox->width  = clock->charset->cell_w * strlen(clock->timebuff);
	dialbox->height = clock->charset->cell_h;
	
	dialbox->x      = (console->width  - dialbox->width)  >> 1;
	dialbox->y      = (console->height - dialbox->height) >> 1;

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
	Clock_apply_colors(clock);

	if (clock->has_frame)
		Clock_draw_frame(clock);
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
		.start             = Clock_trigger_update,
		.get_time          = Clock_get_time_full,
		.dialbox           = { 0 },
		.framebox          = { 0 },
	};

	clock.start(&clock); // refers to Clock_trigger_update

	while (App_listen(&clock))
	{
		clock.get_time(&clock);
		Clock_print(&clock);

		Sleep(50);
	}
	
	Console_free(&console);

	return 0;
}