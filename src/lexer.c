#ifdef MAGNA_MODULE
#include "lexer.h"
#endif

void tokenize_next(enum tok_type *op, const char **lex)
{
	if (*op != TOK_NOP)
		(*lex)++;
	while (**lex == ' ') (*lex)++;

	switch (**lex) {
	case '=':
		*op = TOK_EQU;
		break;
	case '+':
		*op = TOK_ADD;
		break;
	case '-':
		*op = TOK_SUB;
		break;
	case '*':
		*op = TOK_MUL;
		break;
	case '/':
		*op = TOK_DIV;
		break;
	case '^':
		*op = TOK_POW;
		break;
	case '(':
		*op = TOK_OPN;
		break;
	case ')':
		*op = TOK_CLS;
		break;

	case ';':
	case '\0':
		*op = TOK_NOP;
		break;
	
	default:
		*op = TOK_ARG;
		break;
	}
}
