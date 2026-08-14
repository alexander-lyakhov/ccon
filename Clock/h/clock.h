#ifndef CLOCK_H__
#define CLOCK_H__

#include <stdint.h>
#include "console.h"
#include "charset.h"

typedef struct _Bounds {
	uint16_t x;
	uint16_t y;
	uint16_t width;
	uint16_t height;

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

	Bounds dialbox;
	Bounds framebox;

	struct tm time;
	char timebuff[32];

} Clock;

#endif
