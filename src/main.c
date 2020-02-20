#include "lexer.h"
#include "parser.h"

int main()
{
	parse("1 + 2 * 3 + (4 + 2 - (3 - 1)) * 3 + 4^3^2 * 7");

	return 0;
}
