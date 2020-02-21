#include "chartools.h"
#include "lexer.h"
#include "parser.h"

/*int main () {
	struct token tok;
	init_char_tab();
	tok.lex.begin = tok.lex.end = "Hello 123 world!";
	
	tokenize_next(&tok);
	tokenize_next(&tok);	

	printf("%i\n", tok.val.i);
	return 0;
}*/

int main(int argc, char *argv[])
{
	init_char_tab();
	if (argc < 2)
		printf("Not enough command line arguments.");
	else
		parse(argv[1]);
	/* parse("1 + 2 * 3 + (4 + 2 - (3 - 1)) * 3 + 4^^3^^2 * 7"); */

	return 0;
}
