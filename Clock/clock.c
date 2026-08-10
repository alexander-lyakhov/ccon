/*
  кФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФП
  Г                                                                                  Г
  Г   ллллллл  ллллллл             ллллллл   лл   лл             ллллллл   ллллллл   Г
  Г   лл   лл       лл     ллл          лл   лл   лл     ллл     лл        лл        Г
  Г   лл   лл  ллллллл               ллллл   ллллллл             ллллллл   ллллллл   Г
  Г   лл   лл  лл          ллл          лл        лл     ллл          лл   лл   лл   Г
  Г   ллллллл  ллллллл             ллллллл        лл             ллллллл   ллллллл   Г
  Г                                                                                  Г
  РФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФй
*/

#include <stdio.h>
#include <stdint.h>
#include <conio.h>
#include <string.h>
#include <windows.h>
#include <time.h>
#include "h/charset.h"

// #define CHARSET_01_IMPLEMENTATION
#include "h/charset01.h"

// #define CHARSET_02_IMPLEMENTATION
#include "h/charset02.h"

#define CHARSET_03_IMPLEMENTATION
#include "h/charset03.h"

#define PUSH_ADDR(addr) void *tmp = (addr);
#define  POP_ADDR(addr) (addr) = tmp;
#define  INC_LINE(addr) (addr) += console->width;

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

typedef struct _Frame {
	uint16_t screen_x;
	uint16_t screen_y;
	uint16_t width;
	uint16_t height;
	uint16_t padding_x;
	uint16_t padding_y;

} Frame;

typedef struct _Clock {
	Console *console;
	Charset *charset;
	Frame *frame;
	struct tm time;
	char timebuff[32];

} Clock;

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

// =============================================================================
// @@@ + Frame_create
// =============================================================================
Frame Frame_create(Clock *clock, const char *format)
{
	Console *console = clock->console;
	Charset *charset = clock->charset;

	uint16_t padding_x = 4;
	uint16_t padding_y = 3;

	uint16_t text_width  = charset->cell_w * strlen(format);
	uint16_t text_height = charset->cell_h;

	uint16_t frame_width  = text_width  + padding_x * 2;
	uint16_t frame_height = text_height + padding_y * 2;

	uint16_t frame_screen_x = (console->width  - frame_width)  >> 1;
	uint16_t frame_screen_y = (console->height - frame_height) >> 1;

	return (Frame) {
		.screen_x  = frame_screen_x,
		.screen_y  = frame_screen_y,
		.width     = frame_width,
		.height    = frame_height,
		.padding_x = padding_x,
		.padding_y = padding_y,
	};
}

// =============================================================================
// @@@ + Clock_render
// =============================================================================
void Clock_render(Console *console)
{
	WriteConsoleOutputCharacter(
		console->handle,
		console->buff,
		console->size,
		(COORD){0, 0},
		&console->written
	);

	WriteConsoleOutputAttribute(
		console->handle,
		console->attrs,
		console->size,
		(COORD){0, 0},
		&console->written
	);
}

// =============================================================================
// @@@ + Clock_print
// =============================================================================
void Clock_print(Clock *clock, const char *str)
{
	Console *console = clock->console;
	Charset *charset = clock->charset;

	int screenx = (console->width  - charset->cell_w * strlen(str)) >> 1;
	int screeny = (console->height - charset->cell_h) >> 1;

	char *dest = console->buff + screeny * console->width + screenx;

	for (int line = 0; line < charset->cell_h; line++)
	{
		PUSH_ADDR(dest);

		for (const char *p = str; *p; p++)
		{
			int index = *p - 48;
			memcpy(dest, charset->data[index][line], charset->cell_w);
			dest += charset->cell_w;
		}

		POP_ADDR(dest);
		INC_LINE(dest);
	}

	Clock_render(console);
}

void Clock_print_time(Clock *clock)
{
	time_t now = time(NULL);
	localtime_s(&clock->time, &now);

	sprintf(clock->timebuff, "%02d:%02d:%02d",
		clock->time.tm_hour,
		clock->time.tm_min,
		clock->time.tm_sec
	);
	Clock_print(clock, clock->timebuff);
}

// =============================================================================
// @@@ + Clock_draw_frame
// =============================================================================
void Clock_draw_frame(Clock *clock, const char *str)
{
	Frame *frame     = clock->frame;
	Console *console = clock->console;

	int first_row = 0;
	int final_row = frame->height - 1;

	char *dest = console->buff + frame->screen_x + frame->screen_y * console->width;
	
	for (int line = 0; line < frame->height; line++)
	{
		PUSH_ADDR(dest);

		if (line == first_row) {
			dest[0]                = 'к';
			dest[frame->width - 1] = 'П';
		}
		else if (line == final_row) {
			dest[0]                = 'Р';
			dest[frame->width - 1] = 'й';
		}
		else {
			dest[0]                = 'Г';
			dest[frame->width - 1] = 'Г';
		}

		for (int x = 0; x < frame->width - 2; x++) {
			dest[x + 1] = (!line || line == frame->height - 1) ? 'Ф' : ' ';
		}

		POP_ADDR(dest);
		INC_LINE(dest);
	}
}

// =============================================================================
// @@@ + app_listen
// =============================================================================
uint16_t App_listen(Console *console)
{
	if (_kbhit())
	{
		char key = _getch();
		if (key == 27 || ((key | 32) == 'q')) return 0;
	}

	return 1;
}

int main()
{
	// system("cls");

	const char *time_format = "##:##:##";

	// Charset charset = charset02;

	Console console = Console_create();

	Clock clock = {
		.console = &console,
		.charset = &charset03,
	};
	Frame frame = Frame_create(&clock, time_format);
	clock.frame = &frame;

	// printf("w = %d\n", charset.cell_w);
	// printf("h = %d\n", charset.cell_h);

	// Clock_draw_frame(&clock, "12:48:03");
	// Clock_print(&clock, "02:48:03");
	// Clock_print_time(&clock);
	
	while (App_listen(&console))
	{
		Clock_print_time(&clock);

		Sleep(50);
	}
	
	Console_free(&console);

	return 0;
}