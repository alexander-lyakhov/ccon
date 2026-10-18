#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>

#define SIZE 80

int main()
{
	system("cls");
	// printf("Hello !!!");

	FILE *file = fopen("ASCII-03.txt", "r");

	if (!file) {
		perror("File open error");
		exit(1);
	}

	char *texture = malloc(SIZE * SIZE);
	char str[256];

	for (char *p = texture; ;)
	{
		fgets(str, 256, file);

		if (feof(file))
			break;

		memcpy(p, str, SIZE);
		p += SIZE;

		memcpy(p, str, SIZE);
		p += SIZE;

		// printf("%.*s", SIZE, p);
	}

	
	char *p = texture;
	for (int i = 0; i++ < SIZE; p += SIZE)
	{
		printf("%.*s\n", SIZE, p);
	}
	
	free(texture);
	// _getch();

	return 0;
}