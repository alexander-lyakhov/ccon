#ifndef DEFS_H__
#define DEFS_H__

#define CURSOR_INFO(clock) CONSOLE_CURSOR_INFO cursorInfo; GetConsoleCursorInfo((clock)->console->handle, &cursorInfo);
#define CURSOR_HIDE(clock) cursorInfo.bVisible = 0; SetConsoleCursorInfo((clock)->console->handle, &cursorInfo);
#define CURSOR_SHOW(clock) cursorInfo.bVisible = 1; SetConsoleCursorInfo((clock)->console->handle, &cursorInfo);

#define LOOP_TO(value) for (size_t index = 0; index < (value); index++)

#define PUSH_ADDR(addr) void *tmp = (addr);
#define  POP_ADDR(addr) (addr) = tmp;
#define  INC_LINE(addr) (addr) += console->width;

#endif
