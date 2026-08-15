#ifndef CLOCK_H__
#define CLOCK_H__

#include <stdint.h>
#include "console.h"
#include "charset.h"

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
