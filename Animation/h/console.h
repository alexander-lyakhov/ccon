#ifndef CONSOLE_H__
#define CONSOLE_H__

#include <stdint.h>
#include <minwindef.h>
#include <windows.h>

typedef wchar_t WCHR;

typedef struct _Console {
	HANDLE handle;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	CONSOLE_FONT_INFO cfi;
	DWORD written;

	void* buff;
	WORD* attrs;

	uint16_t width;
	uint16_t height;
	uint16_t size;

	float font_ar;

	void (*mem_alloc)(struct _Console *console);
	void (*mem_fill) (struct _Console *console, const void *fillchar, WORD attr);

} Console;

Console Console_create();
Console Console_createW();
void    Console_mem_alloc   (Console *console);
void    Console_mem_fill    (Console *console, const void *fillchar, WORD attr);
void    Console_mem_free    (Console *console);
void    Console_mem_reset   (Console *console);
uint8_t Console_check_resize(Console *console);

// #define CONSOLE_IMPLEMENTATION
#ifdef CONSOLE_IMPLEMENTATION

static void _Console_mem_alloc (Console *console);
static void _Console_mem_allocW(Console *console);

static void _Console_mem_fill (Console *console, const void *fillchar, WORD attr);
static void _Console_mem_fillW(Console *console, const void *fillchar, WORD attr);

// ================================================================================
// @@@ + Console_create
// ================================================================================
static Console* _Console_create()
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_SCREEN_BUFFER_INFO csbi;
	CONSOLE_FONT_INFO cfi;

	GetConsoleScreenBufferInfo(
		handle, &csbi
	);
	GetCurrentConsoleFont(
		handle, 1, &cfi
	);
	COORD FONT = GetConsoleFontSize(
		handle, cfi.nFont
	);

	uint16_t width  = csbi.srWindow.Right + 1;
	uint16_t height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
	uint16_t size   = width * height;

	Console *console = malloc(sizeof(Console));
	console->handle  = handle;
	console->csbi    = csbi;
	console->cfi     = cfi;
	console->width   = width;
	console->height  = height;
	console->size    = size;
	console->font_ar = (float)FONT.X / FONT.Y;

	return console;
}

Console Console_create()
{
	Console *console = _Console_create();

	console->mem_alloc = _Console_mem_alloc;
	console->mem_fill  = _Console_mem_fill;

	console->mem_alloc(console);
	
	return *console;
}

Console Console_createW()
{
	Console *console = _Console_create();

	console->mem_alloc = _Console_mem_allocW;
	console->mem_fill  = _Console_mem_fillW;

	console->mem_alloc(console);
	
	return *console;
}

// =============================================================================
// @@@ + Console_mem_alloc
// =============================================================================
static void _Console_mem_alloc(Console *console)
{
	console->buff  = malloc(console->size * sizeof(char));
	console->attrs = malloc(console->size * sizeof(WORD));
}

static void _Console_mem_allocW(Console *console)
{
	console->buff  = malloc(console->size * sizeof(WCHR));
	console->attrs = malloc(console->size * sizeof(WORD));
}

void Console_mem_alloc(Console *console) {
	console->mem_alloc(console);
}

// =============================================================================
// @@@ + Console_mem_fill
// =============================================================================
static void _Console_mem_fill(Console *console, const void *fillchar, WORD attr)
{
	char *b = console->buff;
	WORD *a = console->attrs;

	for (size_t i = 0; i < console->size; i++)
	{
		*b++ = *(char*)fillchar;
		*a++ = attr;
	}
}

static void _Console_mem_fillW(Console *console, const void *fillchar, WORD attr)
{
	WCHR *b = console->buff;
	WORD *a = console->attrs;

	for (size_t i = 0; i < console->size; i++)
	{
		*b++ = *(WCHR*)fillchar;
		*a++ = attr;
	}
}

void Console_mem_fill(Console *console, const void *fillchar, WORD attr) {
	console->mem_fill(console, fillchar, attr);
}

// =============================================================================
// @@@ + Console_mem_free
// =============================================================================
void Console_mem_free(Console *console)
{
	free(console->buff);
	free(console->attrs);

	console->buff  = NULL;
	console->attrs = NULL;
}

// =============================================================================
// @@@ + Console_mem_reset
// =============================================================================
void Console_mem_reset(Console *console)
{
	Console_mem_free(console);
	Console_mem_alloc(console);
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

#endif
#endif
