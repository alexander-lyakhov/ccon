#include <stdio.h>
#include <conio.h>
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
	Console_mem_fill(&console, "%", 0x03);

	int cx = console.width  >> 1;
	int cy = console.height >> 1;

	/*for (int x = 0; x < console.width; x++)
	{
		for (int y = 0; y < console.width; y++)
		{
		}
	}*/

	app_render(&console);

	_getch();

	Console_mem_free(&console);

	return 0;
}