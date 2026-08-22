#include <stdio.h>
#include <stdint.h>
#include <conio.h>
#include <string.h>
#include <windows.h>
#include <time.h>
#include <math.h>
#include "h/defs.h"

#define CONSOLE_IMPLEMENTATION
	#include "h/console.h"

#define CLOCK_IMPLEMENTATION
	#include "h/clock.h"

#include "h/charset.h"

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

#define CHARSET_LINES_IMPLEMENTATION
	#include "h/charset-lines.h"

#define CHARSET_ELECTRONOCA_IMPLEMENTATION
	#include "h/charset-electronika-01.h"

// =============================================================================
// @@@ + app_listen
// =============================================================================
uint16_t App_listen(Clock *clock)
{
	if (Console_check_resize(clock->console))
	{
		Console_reset(clock->console);
		Clock_trigger_update(clock);
		return 1;
	}

	if (_kbhit())
	{
		char key = _getch();

		if (((key | 32) == 'q' || key == 27))
			return 0;

		if ((key | 32) == 'f')
			clock->has_frame ^= 1;

		if ((key | 32) == 's')
		{
			clock->get_time = clock->get_time == Clock_get_time_full
				? Clock_get_time_short
				: Clock_get_time_full;
		}

		if (key >= '1' && key <= '9')
		{
			int index = (key - 49) % clock->charset_list_size;
			clock->charset = &clock->charset_list[index];
		}
	}

	Clock_trigger_update(clock);
	return 1;
}

int main()
{
	system("cls"); // required to be able to work in Conemu

	Charset charset_list[] = {
		charset01,
		charset02,
		charset03,
		charset04,
		charset05,
		charset_lines,
		charset_electronika,
	};

	Console console = Console_createW();

	Clock clock = {
		.console           = &console,
		.charset           = &charset_list[1],
		.charset_list      = charset_list,
		.charset_list_size = sizeof(charset_list) / sizeof(*charset_list),
		.padding_x         = 6,
		.padding_y         = 3,
		.has_frame         = 1,
		.clock_color       = 0x07,
		.frame_color       = 0x08,
		.start             = Clock_trigger_update,
		.get_time          = Clock_get_time_full,
		.digitbox          = { 0 },
		.framebox          = { 0 },
	};

	CURSOR_INFO(&clock);
	CURSOR_HIDE(&clock);

	clock.start(&clock); // refers to Clock_trigger_update

	while (App_listen(&clock))
	{
		clock.get_time(&clock);
		Clock_print(&clock);

		Sleep(50);
	}
	
	Console_free(&console);
	CURSOR_SHOW(&clock);

	return 0;
}
