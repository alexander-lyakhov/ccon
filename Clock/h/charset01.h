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

#define DIG_W 9
#define DIG_H 5

typedef char *Digit[];
typedef char **Charset[];

extern Charset charset;

#ifdef CHARSET_IMPLEMENTATION

	static Digit zero = {
		" ллллллл ",
		" лл   лл ",
		" лл   лл ",
		" лл   лл ",
		" ллллллл ",
	};

	static Digit one = {
		"      лл ",
		"      лл ",
		"      лл ",
		"      лл ",
		"      лл ",
	};

	static Digit two = {
		" ллллллл ",
		"      лл ",
		" ллллллл ",
		" лл      ",
		" ллллллл ",
	};

	static Digit three = {
		" ллллллл ",
		"      лл ",
		"   ллллл ",
		"      лл ",
		" ллллллл ",
	};

	static Digit four = {
		" лл   лл ",
		" лл   лл ",
		" ллллллл ",
		"      лл ",
		"      лл ",
	};

	static Digit five = {
		" ллллллл ",
		" лл      ",
		" ллллллл ",
		"      лл ",
		" ллллллл ",
	};

	static Digit six = {
		" ллллллл ",
		" лл      ",
		" ллллллл ",
		" лл   лл ",
		" ллллллл ",
	};

	static Digit seven = {
		" ллллллл ",
		"      лл ",
		"      лл ",
		"      лл ",
		"      лл ",
	};

	static Digit eight = {
		" ллллллл ",
		" лл   лл ",
		" ллллллл ",
		" лл   лл ",
		" ллллллл ",
	};

	static Digit nine = {
		" ллллллл ",
		" лл   лл ",
		" ллллллл ",
		"      лл ",
		" ллллллл ",
	};

	static Digit dots = {
		"         ",
		"   ллл   ",
		"         ",
		"   ллл   ",
		"         ",
	};

	Charset charset = { zero, one, two, three, four, five, six, seven, eight, nine, dots };

#endif
#endif
