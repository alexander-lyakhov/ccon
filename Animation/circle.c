#include <stdio.h>
#include <conio.h>
#include <windows.h>

#define CONSOLE_IMPLEMENTATION
#include "h/console.h"

void app_render(Console *console)
{
	WriteConsoleOutputCharacter(console->handle, console->buff, console->size, (COORD){0, 0}, &console->written);
	WriteConsoleOutputAttribute(console->handle, console->attrs, console->size, (COORD){0, 0}, &console->written);
}

int main()
{
	system("cls");

	Console console = Console_create();
	Console_fill(&console, "%", 0x03);

	app_render(&console);

	_getch();

	Console_free(&console);

	return 0;
}