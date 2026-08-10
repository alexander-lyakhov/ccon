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

#ifndef CHARSET_01_H_
#define CHARSET_01_H_

#include <stdio.h>
#include "charset.h"

#ifdef CHARSET_01_IMPLEMENTATION

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
	
	Charset charset01 = {
		.cell_w = 9,
		.cell_h = 5,
		.data = { zero, one, two, three, four, five, six, seven, eight, nine, dots },
	};

#endif
#endif
