#ifndef MACROS_H__
#define MACROS_H__

#define CURSOR_INFO(env) CONSOLE_CURSOR_INFO cursorInfo; GetConsoleCursorInfo((env)->console->handle, &cursorInfo);
#define CURSOR_HIDE(env) cursorInfo.bVisible = 0; SetConsoleCursorInfo((env)->console->handle, &cursorInfo);
#define CURSOR_SHOW(env) cursorInfo.bVisible = 1; SetConsoleCursorInfo((env)->console->handle, &cursorInfo);

#define LOOP_TO(value) for (size_t index = 0; index < (value); index++)

#define PUSH_ADDR(addr) void *tmp = (addr);
#define  POP_ADDR(addr) (addr) = tmp;
#define  INC_LINE(addr) (addr) += console->width;

#endif
