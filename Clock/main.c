#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>
#include <windows.h>
// #include <fcntl.h>
// #include <io.h>

#define WCHAR_FIX_IMPLEMENTATION
#include "wcharfix.h"

int main()
{
	setlocale(LC_ALL, "");

	// _setmode(_fileno(stdout), _O_U16TEXT);
	WCHAR_FIX;

	wchar_t str[] = L"▓▓▓▓";

	for (int i = 0; str[i]; i++)
	{
		printf("U+%04X\n", (unsigned)str[i]);
	}

	wprintf(L"%ls\n", str);
	wprintf(L"%ls\n", L"█████");
	wprintf(L"Ω\n");

	wprint(L"▓▓▓▓\n");

	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

	DWORD written;

	WriteConsoleW(h, L"▓▓▓▓", 5, &written, NULL);
	return 0;
}