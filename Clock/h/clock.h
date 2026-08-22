#ifndef CLOCK_H__
#define CLOCK_H__

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <windows.h>
#include <math.h>
#include <time.h>

#include "defs.h"
#include "charset.h"
#include "console.h"

#define FRAME_WIDTH  clock->framebox.width
#define FRAME_HEIGHT clock->framebox.height
#define FRAME_CHARS  clock->framebox.target_chars
#define FRAME_ATTRS  clock->framebox.target_attrs

#define DIGITS_WIDTH  clock->digitbox.width
#define DIGITS_HEIGHT clock->digitbox.height
#define DIGITS_CHARS  clock->digitbox.target_chars
#define DIGITS_ATTRS  clock->digitbox.target_attrs

#define TIME_LEN strlen(clock->timebuff)

#define CONSOLE_WIDTH  clock->console->width
#define CONSOLE_HEIGHT clock->console->height

typedef struct _Bounds {
	// !!! should be signed !!!
	int16_t x;
	int16_t y;
	int16_t width;
	int16_t height;

	void *target_chars;
	void *target_attrs;

} Bounds;

typedef struct _Clock {
	Console *console;
	Charset *charset;
	Charset *charset_list;
	uint8_t charset_list_size;

	uint8_t padding_x;
	uint8_t padding_y;

	uint8_t has_frame;

	void (*start)   (struct _Clock *clock);
	void (*get_time)(struct _Clock *clock);
	
	WORD clock_color;
	WORD frame_color;

	Bounds digitbox;
	Bounds framebox;

	struct tm time;
	char timebuff[32];

} Clock;

uint8_t Clock_get_bounds     (Clock *clock);
void    Clock_apply_colors   (Clock *clock);
void    Clock_draw_frame     (Clock *clock);
void    Clock_get_time_full  (Clock *clock);
void    Clock_get_time_short (Clock *clock);
void    Clock_trigger_update (Clock *clock);
void    Clock_print          (Clock *clock);
void    Clock_render         (Console *console);

// #define CLOCK_IMPLEMENTATION
#ifdef CLOCK_IMPLEMENTATION

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

	framebox->target_chars = (WCHR*)console->buff  + framebox->x + framebox->y * CONSOLE_WIDTH;
	framebox->target_attrs = console->attrs + framebox->x + framebox->y * CONSOLE_WIDTH;

	if (FRAME_WIDTH >= CONSOLE_WIDTH || FRAME_HEIGHT >= CONSOLE_HEIGHT)
		clock->has_frame = 0;

	// DIGITS

	digitbox->width  = clock->charset->cell_w * TIME_LEN;
	digitbox->height = clock->charset->cell_h;
	
	digitbox->x      = (CONSOLE_WIDTH  - digitbox->width)  >> 1;
	digitbox->y      = (CONSOLE_HEIGHT - digitbox->height) >> 1;

	DIGITS_CHARS     = (WCHR*)console->buff  + digitbox->y * CONSOLE_WIDTH + digitbox->x;
	DIGITS_ATTRS     = console->attrs + digitbox->y * CONSOLE_WIDTH + digitbox->x;

	return DIGITS_WIDTH >= CONSOLE_WIDTH ? 0 : 1;
}

// =============================================================================
// @@@ + Clock_apply_colors
// =============================================================================
void Clock_apply_colors(Clock *clock)
{
	Console *console = clock->console;

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
// @@@ + Clock_draw_frame
// =============================================================================
void Clock_draw_frame(Clock *clock)
{
	Console *console = clock->console;

	uint16_t first_row = 0;
	uint16_t final_row = FRAME_HEIGHT - 1;

	WCHR *target_chars = FRAME_CHARS;
	WORD *target_attrs = FRAME_ATTRS;

	for (int line = 0; line < FRAME_HEIGHT; line++)
	{
		PUSH_ADDR(target_chars);

		if (line == first_row) {
			target_chars[0]               = L'┌';
			target_chars[FRAME_WIDTH - 1] = L'┐';
		}
		else if (line == final_row) {
			target_chars[0]               = L'└';
			target_chars[FRAME_WIDTH - 1] = L'┘';
		}
		else {
			target_chars[0]               = L'│';
			target_chars[FRAME_WIDTH - 1] = L'│';
		}

		for (int x = 0; x < FRAME_WIDTH - 2; x++) {
			target_chars[x + 1] = (!line || line == FRAME_HEIGHT - 1) ? L'─' : L' ';
		}

		POP_ADDR(target_chars);
		INC_LINE(target_chars);
	}
}

// =============================================================================
// @@@ + Clock_get_time_full
// =============================================================================
void Clock_get_time_full(Clock *clock)
{
	time_t now = time(NULL);
	localtime_s(&clock->time, &now);

	sprintf(
		clock->timebuff,
		clock->time.tm_sec & 1 ? "%02d:%02d:%02d": "%02d;%02d;%02d",
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

	sprintf(
		clock->timebuff,
		clock->time.tm_sec & 1 ? "%02d:%02d": "%02d;%02d",
		clock->time.tm_hour,
		clock->time.tm_min
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

	WCHR *target_chars = (WCHR*)DIGITS_CHARS;
	
	for (int line = 0; line < charset->cell_h; line++)
	{
		PUSH_ADDR(target_chars);

		for (const char *p = clock->timebuff; *p; p++)
		{
			int index = *p - 47;

			memcpy(
				target_chars,
				charset->data[index][line],
				charset->cell_w * sizeof(WCHR)
			);
			target_chars += charset->cell_w;
		}

		POP_ADDR(target_chars);
		INC_LINE(target_chars);
	}

	Clock_render(console);
}

// =============================================================================
// @@@ + Clock_render
// =============================================================================
void Clock_render(Console *console)
{
	WriteConsoleOutputCharacterW(
		console->handle,
		(WCHR*)console->buff,
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
// @@@ + Clock_trigger_update
// =============================================================================
void Clock_trigger_update(Clock *clock)
{
	CURSOR_INFO(clock);
	CURSOR_HIDE(clock);

	clock->console->Console_fill(clock->console, L"%", 0x03);

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

#endif
#endif
