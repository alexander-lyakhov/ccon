/*
  кФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФП
  Г                                                                                  Г
  Г   ллллллл  ллллллл             ллллллл   лл   лл             ллллллл   ллллллл   Г
  Г   лл   лл       лл     ллл          лл   лл   лл     ллл     лл        лл        Г
  Г   лл   лл  ллллллл               ллллл   ллллллл             ллллллл   ллллллл   Г
  Г   лл   лл  лл          ллл          лл        лл     ллл          лл   лл   лл   Г
  Г   ллллллл  ллллллл             ллллллл        лл             ллллллл   ллллллл   Г
  Г                                                                                  Г
  РФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФй
*/

#ifndef CHARSET_H_
#define CHARSET_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef const char *Cell[];
typedef const char **Cells[];

typedef struct {
	size_t cell_w;
	size_t cell_h;
	// const char ***data; // the same as Cells
	Cells data;

} Charset;

// extern Charset charset;
// extern Cells cells;

// #define CHARSET_IMPLEMENTATION
#ifdef CHARSET_IMPLEMENTATION

	static Cell zero = {
		" ллллллл ",
		" лл   лл ",
		" лл   лл ",
		" лл   лл ",
		" ллллллл ",
	};

	static Cell one = {
		"      лл ",
		"      лл ",
		"      лл ",
		"      лл ",
		"      лл ",
	};

	static Cell two = {
		" ллллллл ",
		"      лл ",
		" ллллллл ",
		" лл      ",
		" ллллллл ",
	};

	static Cell three = {
		" ллллллл ",
		"      лл ",
		"   ллллл ",
		"      лл ",
		" ллллллл ",
	};

	static Cell four = {
		" лл   лл ",
		" лл   лл ",
		" ллллллл ",
		"      лл ",
		"      лл ",
	};

	static Cell five = {
		" ллллллл ",
		" лл      ",
		" ллллллл ",
		"      лл ",
		" ллллллл ",
	};

	static Cell six = {
		" ллллллл ",
		" лл      ",
		" ллллллл ",
		" лл   лл ",
		" ллллллл ",
	};

	static Cell seven = {
		" ллллллл ",
		"      лл ",
		"      лл ",
		"      лл ",
		"      лл ",
	};

	static Cell eight = {
		" ллллллл ",
		" лл   лл ",
		" ллллллл ",
		" лл   лл ",
		" ллллллл ",
	};

	static Cell nine = {
		" ллллллл ",
		" лл   лл ",
		" ллллллл ",
		"      лл ",
		" ллллллл ",
	};

	static Cell dots = {
		"         ",
		"   ллл   ",
		"         ",
		"   ллл   ",
		"         ",
	};

	// Cells cells = { zero, one, two, three, four, five, six, seven, eight, nine, dots };
	
	Charset charset = {
		.cell_w = 9,
		.cell_h = 5,
		.data = { zero, one, two, three, four, five, six, seven, eight, nine, dots },
	};
#endif
#endif
