#include <stdio.h>
#include <conio.h>
#include <windows.h>

#define SIZE 80

int main()
{
	system("cls");
	printf("Hello !!!");

	char *texture = malloc(SIZE * SIZE);

	free(texture);
	_getch();

	return 0;
}