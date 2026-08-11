#ifndef FRAME_H__
#define FRAME_H__

#include <stdint.h>
#include "clock.h"
#include "console.h"

typedef struct _Frame {
	uint16_t screen_x;
	uint16_t screen_y;
	uint16_t width;
	uint16_t height;
	uint16_t padding_x;
	uint16_t padding_y;

} Frame;

#endif
