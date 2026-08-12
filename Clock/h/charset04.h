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

#ifndef CHARSET_04_H_
#define CHARSET_04_H_

#include "charset.h"

#define CELL_NAME(name) cell_ ## 04 ## _ ## name
#define CHARSET_NAME    charset_ ## 04

#ifdef CHARSET_04_IMPLEMENTATION

	static Cell CELL_NAME(zero) = {
		" ллллллл ",
		" лл   лл ",
		" лл   лл ",
		" лл   лл ",
		" ллллллл ",
	};

	static Cell CELL_NAME(one) = {
		"      лл ",
		"      лл ",
		"      лл ",
		"      лл ",
		"      лл ",
	};

	static Cell CELL_NAME(two) = {
		" ллллллл ",
		"      лл ",
		" ллллллл ",
		" лл      ",
		" ллллллл ",
	};

	static Cell CELL_NAME(three) = {
		" ллллллл ",
		"      лл ",
		"   ллллл ",
		"      лл ",
		" ллллллл ",
	};

	static Cell CELL_NAME(four) = {
		" лл   лл ",
		" лл   лл ",
		" ллллллл ",
		"      лл ",
		"      лл ",
	};

	static Cell CELL_NAME(five) = {
		" ллллллл ",
		" лл      ",
		" ллллллл ",
		"      лл ",
		" ллллллл ",
	};

	static Cell CELL_NAME(six) = {
		" ллллллл ",
		" лл      ",
		" ллллллл ",
		" лл   лл ",
		" ллллллл ",
	};

	static Cell CELL_NAME(seven) = {
		" ллллллл ",
		"      лл ",
		"      лл ",
		"      лл ",
		"      лл ",
	};

	static Cell CELL_NAME(eight) = {
		" ллллллл ",
		" лл   лл ",
		" ллллллл ",
		" лл   лл ",
		" ллллллл ",
	};

	static Cell CELL_NAME(nine) = {
		" ллллллл ",
		" лл   лл ",
		" ллллллл ",
		"      лл ",
		" ллллллл ",
	};

	static Cell CELL_NAME(dots) = {
		"         ",
		"   ллл   ",
		"         ",
		"   ллл   ",
		"         ",
	};

	// Cells cells = { zero, one, two, three, four, five, six, seven, eight, nine, dots };
	
	Charset CHARSET_NAME = {
		.cell_w = 9,
		.cell_h = 5,
		.data = {
			CELL_NAME(zero),
			CELL_NAME(one),
			CELL_NAME(two),
			CELL_NAME(three),
			CELL_NAME(four),
			CELL_NAME(five),
			CELL_NAME(six),
			CELL_NAME(seven),
			CELL_NAME(eight),
			CELL_NAME(nine),
			CELL_NAME(dots)
		},
	};

#endif
#endif
