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

	// int vp_size = fmin(console.width, console.height);
	int vp_size = fmin(floor(console.width * console.font_ar), console.height);
	// int vp_size = 63;

	printf("w = %d, h = %d, vp_size = %d, font_ar = %f",
		console.width,
		console.height,
		vp_size,
		console.font_ar
	);

	_getch();

	size_t index = 0;
	
	/*
	for (int row = 0; row < console.height; row++)
	{
		for (int col = 0; col < console.width; col++)
		{
			float x = (col - cx) * font_ar;
			float y = (row - cy);

			int r = sqrt(x * x + y * y);

			((char*)console.buff)[index++] = r >= (vp_size / 2) ? ' ' : '$';
		}
	}
	*/

	float vpx_factor = vp_size / console.font_ar;
	float vpy_factor = vp_size;
	
	for (int row = 0; row < console.height; row++)
	{
		for (int col = 0; col < console.width; col++)
		{
			float x = ((col - cx) << 1) / (float)vpx_factor;
			float y = ((row - cy) << 1) / (float)vpy_factor;

			float r = sqrt(x * x + y * y);

			((char*)console.buff)[index++] = r >= 1 ? ' ' : '$';
		}
	}
	
	app_render(&console);

	Console_mem_free(&console);

	_getch();

	return 0;
}