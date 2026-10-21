#ifndef APP_H__
#define APP_H__

#include <stdint.h>

#include "./console.h"
#include "./camera.h"
#include "./plane.h"
#include "./cube.h"

typedef struct App {
	Console *console;
	Camera *camera;
	Plane *plane;
	Cube *cube;
	SGE *engine;

	uint8_t is_debug_mode;
} App;

#endif
