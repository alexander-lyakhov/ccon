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
#include "h/clock.h"
#include "h/charset.h"
#include "h/frame.h"

// #define FRAME_IMPLEMENTATION
// #include "h/frame.h"

#define CONSOLE_IMPLEMENTATION
#include "h/console.h"

#define CHARSET_01_IMPLEMENTATION
#include "h/charset01.h"

#define CHARSET_02_IMPLEMENTATION
#include "h/charset02.h"

#define CHARSET_03_IMPLEMENTATION
#include "h/charset03.h"

#define CHARSET_04_IMPLEMENTATION
#include "h/charset04.h"

#define CHARSET_05_IMPLEMENTATION
#include "h/charset05.h"

#define CHARSET_ELECTRONOCA_IMPLEMENTATION
#include "h/charset-electronika.h"

#define PUSH_ADDR(addr) void *tmp = (addr);
#define  POP_ADDR(addr) (addr) = tmp;
#define  INC_LINE(addr) (addr) += console->width;

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
// @@@ + Frame_draw
// =============================================================================
void Frame_draw(Clock *clock, const char *str)
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
	/*
	for (size_t i = 0; i < size; i++)
	{
		buff[i] = ' ';
		attrs[i] = 0x07;
	}
	*/
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
// @@@ + Console_clear
// =============================================================================
void Console_clear(Console *console)
{
	for (size_t i = 0; i < console->size; i++)
	{
		console->buff[i] = ' ';
		console->attrs[i] = 0x07;
	}
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
			int index = *p - 47;
			memcpy(dest, charset->data[index][line], charset->cell_w);
			dest += charset->cell_w;
		}

		POP_ADDR(dest);
		INC_LINE(dest);
	}

	Clock_render(console);
}

// =============================================================================
// @@@ + Clock_get_time
// =============================================================================
void Clock_get_time(Clock *clock)
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
// @@@ + app_listen
// =============================================================================
uint16_t App_listen(Clock *clock)
{
	if (_kbhit())
	{
		char key = _getch();
		if (key == 27 || ((key | 32) == 'q')) return 0;

		if (key >= '1' && key <= '9')
		{
			Console_clear(clock->console);

			int index = (key - 49) % clock->charset_list_size;
			clock->charset = &clock->charset_list[index];
		}
	}

	return 1;
}

int main()
{
	Charset charset_list[] = {
		charset01,
		charset02,
		charset03,
		charset04,
		charset05,
		charset_electronika,
	};

	const char *time_format = "##:##:##";

	Console console = Console_create();
	Console_clear(&console);

	Clock clock = {
		.console           = &console,
		.charset           = &charset_list[2],
		.charset_list      = charset_list,
		.charset_list_size = sizeof(charset_list) / sizeof(*charset_list),
	};
	Frame frame = Frame_create(&clock, time_format);
	clock.frame = &frame;
	
	while (App_listen(&clock))
	{
		Clock_get_time(&clock);
		Sleep(50);
	}
	// printf("%d\n", sizeof(charsets) / sizeof(*charsets));
	
	Console_free(&console);

	return 0;
}