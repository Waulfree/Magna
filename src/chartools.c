#ifdef MAGNA_MODULE
#include "chartools.h"
#endif

char char_tab[0xFF];

void init_char_tab()
{
	unsigned char i;

	/* Add your own rules here, if you like */
	for (i = 0; i < 0xFF; i++)
		char_tab[i] = CHAR_OTHER;
	for (i = 'A'; i <= 'Z'; i++)
		char_tab[i] = char_tab[i | 32] = CHAR_ALPHA;
	for (i = '0'; i <= '9'; i++)
		char_tab[i] = CHAR_NUM;

	char_tab['\0'] = CHAR_NULL;
	char_tab['\n'] = CHAR_LFEED;
	char_tab[' '] = char_tab['\t'] = CHAR_SPACE;
	char_tab['\"'] = CHAR_DQUOTE;
	char_tab['/'] = CHAR_SLASH;

	char_tab['('] = CHAR_LPAREN;
	char_tab[')'] = CHAR_RPAREN;
	char_tab['['] = CHAR_LBRACK;
	char_tab[']'] = CHAR_RBRACK;
	char_tab['='] = CHAR_EQUALS;
	char_tab['$'] = CHAR_DOLLAR;
	char_tab['@'] = CHAR_ATSIGN;
	char_tab[';'] = CHAR_SEMICOL;
}

char get_char_type(const unsigned char c)
{
	return char_tab[c];
}

char char_is_space(const unsigned char c)
{
	return char_tab[c] == CHAR_SPACE;
}

char char_is_lfeed(const unsigned char c)
{
	return char_tab[c] == CHAR_LINE_FEED;
}

char char_is_alpha(const unsigned char c)
{
	return char_tab[c] ==  CHAR_ALPHA;
}

char char_is_num(const unsigned char c)
{
	return char_tab[c] == CHAR_NUM;
}

char char_is_dquote(const unsigned char c)
{
	return char_tab[c] == CHAR_DOUBLE_QUOTE;
}

char char_is_other(const unsigned char c)
{
	return char_tab[c] == CHAR_OTHER;
}

