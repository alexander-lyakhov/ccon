#ifndef CONSOLE_H__
#define CONSOLE_H__

#include <stdint.h>
#include <minwindef.h>
#include <windows.h>

typedef struct _Console {
	HANDLE handle;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD written;

	char* buff;
	WORD* attrs;

	uint16_t width;
	uint16_t height;
	uint16_t size;

} Console;
/*
Console Console_create();

#define CONSOLE_IMPLEMENTATION
#ifdef CONSOLE_IMPLEMENTATION
// ================================================================================
// @@@ + Console_create
// ================================================================================
Console Console_create()
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	GetConsoleScreenBufferInfo(handle, &csbi);

	uint16_t width  = csbi.srWindow.Right + 1;
	uint16_t height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
	uint16_t size   = width * height;

	char *buff = malloc(size);
	WORD *attrs = malloc(sizeof(WORD) * size);

	for (size_t i = 0; i < size; i++)
	{
		buff[i] = ' ';
		attrs[i] = 0x07;
	}

	return (Console) {
		.handle   = handle,
		.width    = width,
		.height   = height,
		.size     = size,
		.buff     = buff,
		.attrs    = attrs,
	};
}

#endif
*/
#endif
