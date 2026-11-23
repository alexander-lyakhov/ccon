#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <conio.h>
#include <windows.h>

#define SEEK_OFFSET 17
#define DEFAULT_FB_SIZE 768
#define  TARGET_FB_SIZE 3072
#define DEFAULT_CHAR_W 8
#define DEFAULT_CHAR_H 8
#define CHAR_W 16
#define CHAR_H 16
#define CHAR_SCALE 1
#define SCROLL_CHAR_WIDTH 8

typedef struct {
	char *font_buff;

} App;

// =============================================================================
// @@@ + App_load_font
// =============================================================================
uint8_t App_load_font(App *app, const char *filename)
{
	FILE *fp = fopen(filename, "rb");

	if (!fp) return 0;

	fseek(fp, SEEK_OFFSET, SEEK_SET); // seek 17 bytes forward

	app->font_buff = malloc(TARGET_FB_SIZE);

	// bool res = fread(app->font_buff, sizeof(char), FB_SIZE, fp) == DEFAULT_FB_SIZE;

	// uint16_t *p = (uint16_t*)app->font_buff;
	char *p = app->font_buff;
	char byte = 0;

	for(int i = 0; i < DEFAULT_FB_SIZE; i++)
	{
		// char byte = fgetc(fp);
		fread(&byte, 1, 1, fp);

		if (byte == EOF)
			break;

		uint16_t word = 0;

		for (int i = 0; i < DEFAULT_CHAR_W; i++)
		{
			char bit = (byte & 128) ? 1 : 0;

			*p |= bit;
			// *p <<= 1;
			// *p |= bit;
			
			byte <<= 1;
		}
		// printf("%d\n", *p);
		p++;
	}

	fclose(fp);

	return 1;
}

// =============================================================================
// @@@ + App_print
// =============================================================================
void App_print(App *app)
{
	char str[9] = { 0 };

	// uint16_t *p = (uint16_t*)app->font_buff;
	char *p = app->font_buff;

	for (int index = 0; index < 32; index += 1)
	{
	    // uint16_t word = *(uint16_t*)(&app->font_buff[index]);

		// for (int i = 0, mask = 0b1000000000000000; mask; i++)
		for (int i = 0, mask = 0b10000000; mask; i++)
		{
			str[i] = *p & mask ? '#' : ' ';
			mask >>= 1;
		}
		puts(str);
		p++;
	}
}

int main()
{
	// system("cls");
	// printf("Hello !!!");

	App app = { 0 };

	App_load_font(&app, "DB_fnt.$C");

	if (app.font_buff)
		free(app.font_buff);

	App_print(&app);

	return 0;
}