#ifndef CLOCK_H__
#define CLOCK_H__

#include <stdint.h>
#include "console.h"
#include "charset.h"

#define FRAME_WIDTH  clock->framebox.width
#define FRAME_HEIGHT clock->framebox.height
#define FRAME_CHARS  clock->framebox.target_chars
#define FRAME_ATTRS  clock->framebox.target_attrs

#define DIGITS_WIDTH  clock->digitbox.width
#define DIGITS_HEIGHT clock->digitbox.height
#define DIGITS_CHARS  clock->digitbox.target_chars
#define DIGITS_ATTRS  clock->digitbox.target_attrs

#define TIME_LEN strlen(clock->timebuff)
// #define TIME_LEN lstrlenW(clock->timebuff)

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

#endif
