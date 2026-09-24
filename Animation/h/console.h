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

	void* buff;
	WORD* attrs;

	uint16_t width;
	uint16_t height;
	uint16_t size;

	void (*alloc)(struct _Console *console);
	void (*fill) (struct _Console *console, const void *fillchar, WORD attr);

} Console;

Console Console_createW();
void    Console_alloc       (Console *console);
void    Console_fill        (Console *console, const void *fillchar, WORD attr);
uint8_t Console_check_resize(Console *console);
void    Console_reset       (Console *console);
void    Console_free        (Console *console);

// #define CONSOLE_IMPLEMENTATION
#ifdef CONSOLE_IMPLEMENTATION

static void _Console_alloc (Console *console);
static void _Console_allocW(Console *console);

static void _Console_fill (Console *console, const void *fillchar, WORD attr);
static void _Console_fillW(Console *console, const void *fillchar, WORD attr);

// ================================================================================
// @@@ + Console_create
// ================================================================================
static Console* _Console_create()
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	GetConsoleScreenBufferInfo(handle, &csbi);

	uint16_t width  = csbi.srWindow.Right + 1;
	uint16_t height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
	uint16_t size   = width * height;

	Console *console = malloc(sizeof(Console));
	console->handle  = handle;
	console->width   = width;
	console->height  = height;
	console->size    = size;

	return console;
}

Console Console_create()
{
	Console *console = _Console_create();

	console->alloc = _Console_alloc;
	console->fill  = _Console_fill;

	console->alloc(console);
	
	return *console;
}

Console Console_createW()
{
	Console *console = _Console_create();

	console->alloc = _Console_allocW;
	console->fill  = _Console_fillW;

	console->alloc(console);
	
	return *console;
}

// =============================================================================
// @@@ + _Console_alloc
// =============================================================================
static void _Console_alloc(Console *console)
{
	console->buff  = malloc(console->size * sizeof(char));
	console->attrs = malloc(console->size * sizeof(WORD));
}

static void _Console_allocW(Console *console)
{
	console->buff  = malloc(console->size * sizeof(WCHR));
	console->attrs = malloc(console->size * sizeof(WORD));
}

// =============================================================================
// @@@ + Console_alloc
// =============================================================================
void Console_alloc(Console *console) {
	console->alloc(console);
}

// =============================================================================
// @@@ + _Console_fill
// =============================================================================
static void _Console_fill(Console *console, const void *fillchar, WORD attr)
{
	char *b = console->buff;
	WORD *a = console->attrs;

	for (size_t i = 0; i < console->size; i++)
	{
		*b++ = *(char*)fillchar;
		*a++ = attr;
	}
}

static void _Console_fillW(Console *console, const void *fillchar, WORD attr)
{
	WCHR *b = console->buff;
	WORD *a = console->attrs;

	for (size_t i = 0; i < console->size; i++)
	{
		*b++ = *(WCHR*)fillchar;
		*a++ = attr;
	}
}

// =============================================================================
// @@@ + Console_fill
// =============================================================================
void Console_fill(Console *console, const void *fillchar, WORD attr) {
	console->fill(console, fillchar, attr);
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
		console->width  = width;
		console->height = height;
		console->size   = width * height;

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
	Console_alloc(console);
}

// =============================================================================
// @@@ + Console_free
// =============================================================================
void Console_free(Console *console)
{
	free(console->buff);
	free(console->attrs);

	console->buff  = NULL;
	console->attrs = NULL;
}

#endif
#endif
