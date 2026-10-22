#ifndef MACROS_H__
#define MACROS_H__

#define CURSOR_INIT(env) CONSOLE_CURSOR_INFO cursorInfo; GetConsoleCursorInfo((env)->console->handle, &cursorInfo);
#define CURSOR_HIDE(env) cursorInfo.bVisible = 0; SetConsoleCursorInfo((env)->console->handle, &cursorInfo);
#define CURSOR_SHOW(env) cursorInfo.bVisible = 1; SetConsoleCursorInfo((env)->console->handle, &cursorInfo);

#define LOOP_TO (value) for (size_t index = 0; index < (value); index++)

#define PUSH_ADDR(addr) void *tmp = (addr);
#define  POP_ADDR(addr) (addr) = tmp;
#define  INC_LINE(addr) (addr) += console->width;

typedef enum _KEYS {
	KEY_LEFT   = 75,
	KEY_RIGHT  = 77,
	KEY_UP     = 72,
	KEY_DOWN   = 80,
	KEY_ENTER  = 13,
	KEY_ESCAPE = 27,
	KEY_SPACE  = 32
} KEYS;

#endif
