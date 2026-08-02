/******************************************************************************

The FIX for wchar unicode symbols output in console ( for MinGW GCC compiler )

*******************************************************************************

HOW TO USE:

	In *.c file define macros WCHAR_FIX_IMPLEMENTATION and include "wcharfix.h"

		#define WCHAR_FIX_IMPLEMENTATION
		#include "wcharfix.h"

	In main function put expression:
		
		WCHAR_FIX;

	Or just use the function:
		
		wprint(const wchar_t* text)

*/

#ifndef _WCHARFIX_H_
#define _WCHARFIX_H_

void wprint(const wchar_t* text);

#ifdef WCHAR_FIX_IMPLEMENTATION
	#include <fcntl.h>
	#include <io.h>

	#define WCHAR_FIX _setmode(_fileno(stdout), _O_U16TEXT);

	void wprint(const wchar_t* text)
	{
		DWORD written;

		WriteConsoleW(
			GetStdHandle(STD_OUTPUT_HANDLE),
			text,
			lstrlenW(text),
			&written,
			NULL
		);
	}
#endif
#endif
