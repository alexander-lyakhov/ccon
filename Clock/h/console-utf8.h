#ifndef CONSOLE_H__
#define CONSOLE_H__

#include <stdint.h>
#include <minwindef.h>
#include <windows.h>

typedef wchar_t WCHR;

typedef struct _Console {
	HANDLE handle;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD written;

	WCHR* buff;
	WORD* attrs;

	uint16_t width;
	uint16_t height;
	uint16_t size;

} Console;

Console Console_create();
void    Console_alloc_buffs (Console *console);
uint8_t Console_check_resize(Console *console);
void    Console_fill_buffs  (Console *console);
void    Console_reset       (Console *console);
void    Console_free        (Console *console);

// #define CONSOLE_IMPLEMENTATION
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

	Console console = {
		.handle   = handle,
		.width    = width,
		.height   = height,
		.size     = size,
	};
	Console_alloc_buffs(&console);

	return console;
}

// =============================================================================
// @@@ + Console_alloc_buffs
// =============================================================================
void Console_alloc_buffs(Console *console)
{
	console->buff  = (WCHR*)malloc(console->size * sizeof(WCHR));
	console->attrs = (WORD*)malloc(console->size * sizeof(WORD));
}

// =============================================================================
// @@@ + Console_fill_buffs
// =============================================================================
void Console_fill_buffs(Console *console)
{
	for (size_t i = 0; i < console->size; i++)
	{
		console->buff[i] = L'%';
		console->attrs[i] = 0x03;
	}
}

// =============================================================================
// @@@ + Console_check_resize
// =============================================================================
uint8_t Console_check_resize(Console *console)
{
	GetConsoleScreenBufferInfo(console->handle, &(console->csbi));

	uint16_t width  = console->csbi.srWindow.Right  + 1;
	uint16_t height = console->csbi.srWindow.Bottom - console->csbi.srWindow.Top + 1;

	if (console->width != width || console->height != height)
	{
		console->width = width;
		console->height = height;
		console->size = width * height;

		return 1; // Console size has been changed
	}

	return 0; // Console size stays the same
}

// =============================================================================
// @@@ + Console_reset
// =============================================================================
void Console_reset(Console *console)
{
	Console_free(console);
	Console_alloc_buffs(console);
	Console_fill_buffs(console);
}

// =============================================================================
// @@@ + Console_free
// =============================================================================
void Console_free(Console *console)
{
	free(console->buff);
	free(console->attrs);

	console->buff = NULL;
	console->attrs = NULL;
}

#endif
#endif
