#ifndef CHARSET_H_
#define CHARSET_H_

typedef const char *Cell[];
typedef const char **Cells[];

typedef struct {
	size_t cell_w;
	size_t cell_h;
	// Cells data;
	const char ***data; // the same as Cells

} Charset;

#endif
