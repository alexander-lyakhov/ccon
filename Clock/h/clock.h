#ifndef CLOCK_H__
#define CLOCK_H__

#include <stdint.h>
#include "console.h"
#include "charset.h"

typedef struct _Clock {
	Console *console;
	Charset *charset;
	Charset *charset_list;
	uint8_t charset_list_size;

	uint8_t padding_x;
	uint8_t padding_y;

	uint8_t has_frame;
	void (*draw_frame)(struct _Clock *clock);
	
	WORD clock_color;
	WORD frame_color;

	struct tm time;
	char timebuff[32];

} Clock;

#endif
