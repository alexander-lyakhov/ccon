#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>

#define SEEK_OFFSET 17
#define FB_SIZE 768

typedef struct {
	char *font_buff;

} App;


// =============================================================================
// @@@ + App_free
// =============================================================================
void App_free(App *app)
{
	if (app->font_buff)
		free(app->font_buff);
}

// =============================================================================
// @@@ + App_print
// =============================================================================
void App_print(App *app)
{
	char str[9] = {0};

	for (int index = 0; index < FB_SIZE; index++)
	{
		for (int i = 0, mask = 0b10000000; mask; i++)
		{
			str[i] = app->font_buff[index] & mask ? '#' : ' ';
			mask >>= 1;
		}
		puts(str);
	}
}

// =============================================================================
// @@@ + App_exit
// =============================================================================
void App_exit(App *app, const char *message)
{
	perror(message);
	App_free(app);
	exit(1);
}

// =============================================================================
// @@@ + App_load
// =============================================================================
uint8_t App_load(App *app, const char *filename)
{
	FILE *fp = fopen(filename, "rb");

	if (!fp) return 0;

	fseek(fp, SEEK_OFFSET, SEEK_SET); // seek 17 bytes forward

	app->font_buff = malloc(FB_SIZE);

	uint8_t res = fread(app->font_buff, sizeof(char), FB_SIZE, fp) == FB_SIZE;

	fclose(fp);

	return res;
}

int main()
{
	system("cls");

	App app = { 0 };

	if (!App_load(&app, "DB_fnt.$C"))
		App_exit(&app, "File open Error");

	App_print(&app);

	App_free(&app);

	_getch();

	return 0;
}