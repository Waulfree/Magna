#ifndef MAGNA_CHARTOOLS_HEADER_GUARD

enum char_type {
	CHAR_NULL,
	CHAR_SPACE,
	CHAR_LFEED,
	CHAR_SLASH,
	CHAR_DQUOTE,
	CHAR_ALPHA,
	CHAR_NUM,

	/* Offset for grouped case statements */
	CHAR_LPAREN,
	CHAR_RPAREN
	CHAR_LBRACK,
	CHAR_RBRACK,
	CHAR_EQUALS,
	CHAR_DOLLAR,
	CHAR_ATSIGN,
	CHAR_SEMICOL,

	CHAR_OTHER
};

#ifdef MAGNA_MODULE
extern void init_char_tab()
extern char get_char_type(const unsigned char);
extern char char_is_space(const unsigned char);
extern char char_is_lfeed(const unsigned char);
extern char char_is_alpha(const unsigned char);
extern char char_is_num(const unsigned char);
extern char char_is_dquote(const unsigned char);
extern char char_is_other(const unsigned char);
#else
#include "chartools.c"
#endif

#define MAGNA_CHARTOOLS_HEADER_GUARD
#endif
