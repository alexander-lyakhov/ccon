#ifndef CLOCK_H__
#define CLOCK_H__

#include <stdint.h>
#include "console.h"
#include "charset.h"
#include "frame.h"

typedef struct {
	Console *console;
	Charset *charset;
	Charset *charset_list;
	uint8_t charset_list_size;

	Frame *frame;

	struct tm time;
	char timebuff[32];

} Clock;

#endif
