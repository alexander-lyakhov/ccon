#ifndef CHARSET_H_
#define CHARSET_H_

typedef const wchar_t *Cell[];
typedef const wchar_t **Cells[];

typedef struct {
	size_t cell_w;
	size_t cell_h;
	// Cells data;
	const wchar_t ***data; // the same as Cells

} Charset;

#endif
