#include <stdio.h>
#include <conio.h>
#include <windows.h>

int main()
{
	system("cls");

	char *str = strdup("12345678");

	while(1)
	{
		printf("%s\n", str);

		char key = _getch();

		if (key == 'q')
			break;

		char c = *str;
		memmove(str, str + 1, strlen(str) - 1);
		str[strlen(str) - 1] = ' ';
	}

	free(str);

	// _getch();

	return 0;
}