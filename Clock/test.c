
#include <stdio.h>

#define CELL(name) simple_ ## name

typedef const char *Cell[];
typedef const char **Cells[];

static Cell CELL(zero) = {
	" ███████ ",
	" ██   ██ ",
	" ██   ██ ",
	" ██   ██ ",
	" ███████ ",
};


int main()
{
    for (int i=0; i<5; i++)
        printf("%s\n", simple_zero[i]);
    

    return 0;
}
