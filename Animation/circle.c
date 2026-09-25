#include <stdio.h>
#include <conio.h>
#include <math.h>
#include <windows.h>

#define CONSOLE_IMPLEMENTATION
#include "h/console.h"

void app_render(Console *console)
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

int main()
{
	system("cls");

	Console console = Console_create();
	Console_mem_fill(&console, " ", 0x03);

	int cx = console.width  >> 1;
	int cy = console.height >> 1;

	size_t index = 0;

	for (int row = 0; row < console.height; row++)
	{
		for (int col = 0; col < console.width; col++)
		{
			float x = col - cx;
			float y = (row - cy) / console.font_ar;

			int r = round(sqrt(x * x + y * y));

			((char*)console.buff)[index++] = r > 24 ? ' ' : '$';
		}
	}

	app_render(&console);

	Console_mem_free(&console);

	// _getch();


	return 0;
}