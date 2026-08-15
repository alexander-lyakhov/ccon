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

#define PUSH_ADDR(addr)  void *tmp = (addr);
#define  POP_ADDR(addr)  (addr) = tmp;
#define  INC_LINE(addr)  (addr) += clock->console->width;
#define   LOOP_TO(value) for (size_t i = 0; i < (value); i++)

#define CURSOR_INFO(clock) CONSOLE_CURSOR_INFO cursorInfo; GetConsoleCursorInfo((clock)->console->handle, &cursorInfo);
#define CURSOR_HIDE(clock) cursorInfo.bVisible = 0; SetConsoleCursorInfo((clock)->console->handle, &cursorInfo);
#define CURSOR_SHOW(clock) cursorInfo.bVisible = 1; SetConsoleCursorInfo((clock)->console->handle, &cursorInfo);

// =============================================================================
// @@@ + Clock_draw_frame
// =============================================================================
void Clock_draw_frame(Clock *clock)
{
	uint16_t first_row = 0;
	uint16_t final_row = FRAME_HEIGHT - 1;

	char *target_chars = FRAME_CHARS;
	WORD *target_attrs = FRAME_ATTRS;

	for (int line = 0; line < FRAME_HEIGHT; line++)
	{
		PUSH_ADDR(target_chars);

		if (line == first_row) {
			target_chars[0]               = 'Ú';
			target_chars[FRAME_WIDTH - 1] = '¿';
		}
		else if (line == final_row) {
			target_chars[0]               = 'À';
			target_chars[FRAME_WIDTH - 1] = 'Ù';
		}
		else {
			target_chars[0]               = '³';
			target_chars[FRAME_WIDTH - 1] = '³';
		}

		for (int x = 0; x < FRAME_WIDTH - 2; x++) {
			target_chars[x + 1] = (!line || line == FRAME_HEIGHT - 1) ? 'Ä' : ' ';
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
	// FRAME COLOR

	WORD *frame_target_attrs = FRAME_ATTRS;

	LOOP_TO(FRAME_HEIGHT)
	{
		PUSH_ADDR(frame_target_attrs);
		
		LOOP_TO(FRAME_WIDTH)
			*frame_target_attrs++ = clock->frame_color;

		POP_ADDR(frame_target_attrs);
		INC_LINE(frame_target_attrs);
	}

	// DIGIT COLOR

	WORD *digit_target_attrs = DIGITS_ATTRS;

	LOOP_TO(DIGITS_HEIGHT)
	{
		PUSH_ADDR(digit_target_attrs);

		LOOP_TO(DIGITS_WIDTH)
			*digit_target_attrs++ = clock->clock_color;

		POP_ADDR(digit_target_attrs);
		INC_LINE(digit_target_attrs);
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

	if (DIGITS_WIDTH > CONSOLE_WIDTH || DIGITS_HEIGHT > CONSOLE_HEIGHT)
		return;

	void *target_chars = DIGITS_CHARS;
	
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
uint8_t Clock_get_bounds(Clock *clock)
{
	clock->get_time(clock);

	Console *console = clock->console;

	Bounds *digitbox  = &clock->digitbox;
	Bounds *framebox = &clock->framebox;

	// FRAME

	framebox->width  = fmin(clock->padding_x * 2 + TIME_LEN * clock->charset->cell_w, CONSOLE_WIDTH);
	framebox->height = fmin(clock->padding_y * 2 + clock->charset->cell_h, CONSOLE_HEIGHT);

	framebox->x = (CONSOLE_WIDTH  - framebox->width)  >> 1;
	framebox->y = (CONSOLE_HEIGHT - framebox->height) >> 1;

	framebox->target_chars = console->buff  + framebox->x + framebox->y * CONSOLE_WIDTH;
	framebox->target_attrs = console->attrs + framebox->x + framebox->y * CONSOLE_WIDTH;

	if (FRAME_WIDTH >= CONSOLE_WIDTH || FRAME_HEIGHT >= CONSOLE_HEIGHT)
		clock->has_frame = 0;

	// DIGITS

	digitbox->width  = clock->charset->cell_w * TIME_LEN;
	digitbox->height = clock->charset->cell_h;
	
	digitbox->x      = (CONSOLE_WIDTH  - digitbox->width)  >> 1;
	digitbox->y      = (CONSOLE_HEIGHT - digitbox->height) >> 1;

	DIGITS_CHARS     = console->buff  + digitbox->y * CONSOLE_WIDTH + digitbox->x;
	DIGITS_ATTRS     = console->attrs + digitbox->y * CONSOLE_WIDTH + digitbox->x;

	return DIGITS_WIDTH >= CONSOLE_WIDTH ? 0 : 1;
}

// =============================================================================
// @@@ + Clock_trigger_update
// =============================================================================
void Clock_trigger_update(Clock *clock)
{
	CURSOR_INFO(clock);
	CURSOR_HIDE(clock);

	Console_fill_buffs(clock->console);

	// If time format takes more space then console width has
	if (!Clock_get_bounds(clock))
	{
		clock->get_time = Clock_get_time_short;
		Clock_get_bounds(clock);
	}

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
			clock->has_frame ^= 1;

		if ((key | 32) == 's')
		{
			clock->get_time = clock->get_time == Clock_get_time_full
				? Clock_get_time_short
				: Clock_get_time_full;
		}

		if (key >= '1' && key <= '9')
		{
			int index = (key - 49) % clock->charset_list_size;
			clock->charset = &clock->charset_list[index];
		}
	}

	Clock_trigger_update(clock);
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
		.digitbox          = { 0 },
		.framebox          = { 0 },
	};

	CURSOR_INFO(&clock);
	CURSOR_HIDE(&clock);

	clock.start(&clock); // refers to Clock_trigger_update

	while (App_listen(&clock))
	{
		clock.get_time(&clock);
		Clock_print(&clock);

		Sleep(50);
	}
	
	Console_free(&console);
	CURSOR_SHOW(&clock);

	return 0;
}